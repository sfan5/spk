#!/bin/bash
eval `cat makeconfig.cfg`
find . -name "*.${OBJEXT}" -delete
find . -name "*~" -delete
