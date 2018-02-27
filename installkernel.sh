echo Backing up previous kernel
tar -cf - /lib/modules /media/boot | zstd -2 > /root/kernel-backup-`date +%Y%m%d-%H%M`.tar.zst

echo Deleting old kernel
rm -fr /media/boot/rk3399-odroidn1-linux.dtb /media/boot/Image /lib/modules/*

echo Installing new kernel
cp arch/arm64/boot/Image arch/arm64/boot/dts/rockchip/rk3399-odroidn1-linux.dtb /media/boot && sync
make modules_install && sync
reboot
