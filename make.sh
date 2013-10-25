#!/bin/sh
eval `cat makeconfig.cfg`
SRCS="extract.c create.c strerror_spk.c"
EXTRA_CFLAGS="-Wall -Iinclude"

echo "CC	main.c"
${CROSS_PREFIX}${CC} -c $CFLAGS $EXTRA_CFLAGS main.c -o main${OBJEXT} || exit 1
OBJS=""
for obj in $SRCS; do
    echo "CC	$obj"
    ${CROSS_PREFIX}${CC} -c $CFLAGS $EXTRA_CFLAGS $obj -o ${obj%.*}$OBJEXT || exit 1
    OBJS="${OBJS} ${obj%.*}$OBJEXT"
done
echo "LD	spk${EXEEXT}"
${CROSS_PREFIX}${CC} ${LDFLAGS} main${OBJEXT} $OBJS -o spk$EXEEXT || exit 1
echo "AR	libspk${LIBEXT}"
${CROSS_PREFIX}ar rcs libspk$LIBEXT $OBJS || exit 1
