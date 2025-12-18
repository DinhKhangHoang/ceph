FROM quay.io/ceph/ceph@sha256:6ac7f923aa1d23b43248ce0ddec7e1388855ee3d00813b52c3172b0b23b37906
RUN dnf update -y && dnf clean all
RUN dnf install -y \
    git