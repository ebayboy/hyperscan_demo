
BUILDDIR=hyperscan/hs_build

ARCH=(core2 corei7 core-avx2)

#FileType=None
#FileType=Debug
FileType=MinSizeRel
#FileType=Release
#FileType=RelWithDebInfo


#ln -s $(pwd)/boost_1_64_0/boost hyperscan/include/boost

CURDIR=$(pwd)

for i in ${ARCH[@]}; do
    rm -fr $BUILDDIR-$i && mkdir $BUILDDIR-$i
    cd $BUILDDIR-$i && cmake ../            \
        -DCMAKE_C_FLAGS="-fPIC -march=$i"   \
        -DCMAKE_CXX_FLAGS="-fPIC -march=$i" \
        -DCMAKE_BUILD_TYPE=$FileType        \
        && make -j 4                        \
        && cd - || exit 1
    cp -af $BUILDDIR-$i/lib/libhs.a ${CURDIR}/libhs-${i}.a
done

