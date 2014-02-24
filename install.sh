#!/bin/bash
eval `cat makeconfig.cfg`
mkdir -p $DESTDIR/usr/local/bin || exit 1
install -s -m 0755 spk $DESTDIR/usr/local/bin || exit 1
mkdir -p $DESTDIR/usr/local/lib || exit 1
install -m 0755 libspk.a $DESTDIR/usr/local/lib || exit 1
mkdir -p $DESTDIR/usr/local/include/spk || exit 1
cd include/spk || exit 1
for f in *; do
	install -m 0644 $f $DESTDIR/usr/local/include/spk/$f || exit 1
done
cd .. || exit 1

