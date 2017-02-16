echo Backing up previous kernel
tar -cf - /lib/modules /media/boot | zstd -2 > /root/kernel-backup-`date +%H%M-%d%m%Y`.tar.zst

echo Deleting old kernel
rm -fr /media/boot/exynos5422-odroidxu3.dtb /media/boot/exynos5422-odroidxu4.dtb /media/boot/meson8b_odroidc.dtb /media/boot/zImage* /media/boot/uImage* /media/boot/uInitrd* /lib/modules/3.10* /lib/modules/3.8.13* /lib/modules/3.4* /lib/modules/4.9*

echo Installing new kernel
cp arch/arm/boot/zImage arch/arm/boot/dts/exynos5422-odroidxu4.dtb /media/boot && sync
make modules_install ARCH=arm && sync
cp .config /boot/config-`make kernelrelease`
update-initramfs -c -k `make kernelrelease`
mkimage -A arm -O linux -T ramdisk -C none -a 0 -e 0 -n uInitrd -d /boot/initrd.img-`make kernelrelease` /boot/uInitrd-`make kernelrelease`
cp /boot/uInitrd-`make kernelrelease` /media/boot/uInitrd
sed -i -e 's/odroidxu3.dtb/odroidxu4.dtb/g' /media/boot/boot.ini
sync && reboot
