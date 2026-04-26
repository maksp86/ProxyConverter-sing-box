#!/bin/bash
set -e

rm -rf $DEST
mkdir -p $DEST

#### copy binary ####
cp $GITHUB_WORKSPACE/build/proxy-converter $DEST

cd download-artifact
cd *$DEST_SUFFIX
tar xvzf artifacts.tgz -C ../../
cd ../..

sudo add-apt-repository universe
sudo apt install libfuse2
sudo apt install patchelf
wget https://github.com/linuxdeploy/linuxdeploy/releases/latest/download/linuxdeploy-$ARCH.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/latest/download/linuxdeploy-plugin-qt-$ARCH.AppImage
chmod +x linuxdeploy-$ARCH.AppImage linuxdeploy-plugin-qt-$ARCH.AppImage

./linuxdeploy-$ARCH.AppImage --appdir $DEST --executable $DEST/proxy-converter --plugin qt
rm linuxdeploy-$ARCH.AppImage linuxdeploy-plugin-qt-$ARCH.AppImage
cd $DEST
rm -r ./usr/translations ./usr/bin ./usr/share ./apprun-hooks

# fix plugins rpath
rm -r ./usr/plugins
mkdir ./usr/plugins
cp -r $QT_PLUGIN_PATH/tls ./usr/plugins
cp -r $QT_PLUGIN_PATH/networkinformation ./usr/plugins

# fix extra libs...
mkdir ./usr/lib2
ls ./usr/lib/
cp ./usr/lib/libQt* ./usr/lib/libicuuc* ./usr/lib/libicui18n* ./usr/lib/libicudata* ./usr/lib2
rm -r ./usr/lib
mv ./usr/lib2 ./usr/lib

# fix lib rpath
cd $DEST
patchelf --set-rpath '$ORIGIN/usr/lib' ./proxy-converter

# handle debug info
objcopy --only-keep-debug $DEST/proxy-converter $DEST/proxy-converter.debug
strip --strip-debug --strip-unneeded $DEST/proxy-converter
objcopy --add-gnu-debuglink=$DEST/proxy-converter.debug $DEST/proxy-converter