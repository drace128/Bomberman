################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
CC3100SupportPackage/spi_cc3100/spi_cc3100.obj: ../CC3100SupportPackage/spi_cc3100/spi_cc3100.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/ccsv7/ccs_base/arm/include" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/G8RTOS" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/BoardSupportPackage/DriverLib" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/BoardSupportPackage/inc" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/CC3100SupportPackage/board" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/CC3100SupportPackage/cc3100_usage" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/CC3100SupportPackage/simplelink/include" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/CC3100SupportPackage/SL_Common" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing/CC3100SupportPackage/spi_cc3100" --include_path="C:/ti/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/Users/brons/Documents/Documents/Fall2017/MicroP2/Projects/Final_Project_Testing" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="CC3100SupportPackage/spi_cc3100/spi_cc3100.d" --obj_directory="CC3100SupportPackage/spi_cc3100" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


