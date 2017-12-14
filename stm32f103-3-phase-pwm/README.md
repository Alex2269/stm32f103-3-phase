# stm32f103-3-phase-pwm

CH1=PA8
CH2=PA9
CH3=PA10
CH1N=PB13
CH2N=PB14
CH3N=PB15

apt install libusb-1* gcc-arm-none-eabi install gdb-arm-none-eabi

wget -O ~/stm32cubemx.zip http://www.st.com/content/ccc/resource/technical/software/sw_development_suite/00/22/e1/cf/b7/83/42/25/stm32cubemx.zip/files/stm32cubemx.zip/jcr:content/translations/en.stm32cubemx.zip

unzip --uoq ~/stm32cubemx.zip -d ~/STM32CubeM-X ; cd ~/STM32CubeM-X

./SetupSTM32CubeMX-4.21.0.linux

runing STM32CubeMX

Load Progect

open project file pwm.ioc

generate code (ctrl+schift+g)

make

make flash

