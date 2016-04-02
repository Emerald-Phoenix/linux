echo Backing up previous kernel
tar -Jcf /root/kernel-backup-`date +%H%M-%d%m%Y`.tar.xz /lib/modules /media/boot

echo Deleting old kernel
rm -fr /media/boot/exynos5422-odroidxu3.dtb /media/boot/meson8b_odroidc.dtb /media/boot/zImage* /media/boot/uImage* /media/boot/uInitrd* /lib/modules/3.10* /lib/modules/3.8.13* /lib/modules/3.4*

echo Installing new kernel
cp arch/arm/boot/zImage arch/arm/boot/dts/exynos5422-odroidxu3.dtb /media/boot && sync
make modules_install ARCH=arm && sync
cp .config /boot/config-`make kernelrelease`
update-initramfs -c -k `make kernelrelease`
mkimage -A arm -O linux -T ramdisk -C none -a 0 -e 0 -n uInitrd -d /boot/initrd.img-`make kernelrelease` /boot/uInitrd-`make kernelrelease`
cp /boot/uInitrd-`make kernelrelease` /media/boot/uInitrd
sync && reboot
