FROM quay.io/ceph/ceph:v18.2.7
RUN cp /usr/bin/radosgw /usr/bin/radosgw.bak
COPY ./build/bin/radosgw /usr/bin/radosgw
COPY ./build/lib/libceph-common.so /lib64/libceph-common.so
COPY ./build/lib/libceph-common.so.2 /lib64/libceph-common.so.2
COPY ./build/lib/librados.so /lib64/librados.so
COPY ./build/lib/librados.so.2 /lib64/librados.so.2