#include <gtest/gtest.h>
#include "common/ceph_context.h"
#include "rgw_common.h"
#include "rgw_opa.h"
#include "rgw_process_env.h"
#include "test_rgw_common.h"

using namespace std;

auto g_cct = new CephContext(CEPH_ENTITY_TYPE_CLIENT);

class CctCleaner {
  CephContext* cct;
public:
  CctCleaner(CephContext* _cct) : cct(_cct) {}
  ~CctCleaner() { cct->put(); }
};

CctCleaner cleaner(g_cct);

// simple identity returning a custom subuser
class TestIdentity : public rgw::auth::Identity {
  std::string sub;
public:
  explicit TestIdentity(const std::string& s = "") : sub(s) {}
  uint32_t get_perms_from_aclspec(const DoutPrefixProvider* dpp, const aclspec_t& aclspec) const override { return 0; }
  bool is_admin_of(const rgw_user& uid) const override { return false; }
  bool is_owner_of(const rgw_user& uid) const override { return false; }
  uint32_t get_perm_mask() const override { return 0; }
  uint32_t get_identity_type() const override { return TYPE_RGW; }
  string get_acct_name() const override { return ""; }
  string get_subuser() const override { return sub; }
  void to_str(std::ostream& out) const override { return; }
  bool is_identity(const boost::container::flat_set<rgw::auth::Principal>& ids) const override { return false; }
};

TEST(RGWOPA, StartsWith)
{
  ASSERT_TRUE(starts_with("prefix_value", "prefix"));
  ASSERT_FALSE(starts_with("noprefix", "prefix"));
}

TEST(RGWOPA, MissingOPAURL)
{
  // create minimal req_state
  RGWProcessEnv penv;
  RGWEnv e;
  req_state s(g_cct, penv, &e, 0);

  // ensure opa url is empty
  s.cct->_conf->rgw_opa_url = "";

  RGWOp* op = nullptr;
  int rc = rgw_opa_authorize(op, &s);
  ASSERT_EQ(rc, -ERR_INVALID_REQUEST);
}

TEST(RGWOPA, SubuserOnlyNoSubuser)
{
  RGWProcessEnv penv;
  RGWEnv e;
  req_state s(g_cct, penv, &e, 0);

  s.cct->_conf->rgw_opa_url = "http://opa";
  s.cct->_conf->rgw_opa_authorize_subuser_only = true;

  RGWOp* op = nullptr;
  int rc = rgw_opa_authorize(op, &s);
  ASSERT_EQ(rc, 0);
}

TEST(RGWOPA, SubuserOnlyPrefixMismatch)
{
  RGWProcessEnv penv;
  RGWEnv e;
  req_state s(g_cct, penv, &e, 0);

  s.cct->_conf->rgw_opa_url = "http://opa";
  s.cct->_conf->rgw_opa_authorize_subuser_only = true;
  s.cct->_conf->rgw_opa_authorize_subuser_id_prefix = "pref_";

  // attach identity with non-matching subuser
  s.auth.identity.reset(new TestIdentity("other_user"));

  RGWOp* op = nullptr;
  int rc = rgw_opa_authorize(op, &s);
  ASSERT_EQ(rc, 0);
}
