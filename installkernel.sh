cp arch/arm/boot/zImage arch/arm/boot/dts/exynos5422-odroidxu3.dtb /media/boot && sync
make modules_install ARCH=arm && sync
cp .config /boot/config-`make kernelrelease`
update-initramfs -c -k `make kernelrelease`
mkimage -A arm -O linux -T ramdisk -C none -a 0 -e 0 -n uInitrd -d /boot/initrd.img-`make kernelrelease` /boot/uInitrd-`make kernelrelease`
cp /boot/uInitrd-`make kernelrelease` /media/boot/uInitrd
sync && reboot
