PROJECT TITLE: LabVIEW and Arduino Software for MISO device 

LICENSE: GNU General Public License v3.0

PUBLICATION REFERENCE: MISO: Microfluidic protein isolation enables single particle cryo-EM structure determination from a single cell colony
https://doi.org/10.1101/2025.01.10.632437

CONTACT: rouslan.efremov@vub.be

SYSTEM REQUIREMENTS

- Windows 11Pro Intel Core i7-13700 2.10 GHz, 32.0 GB RAM, Intel UHD Graphics 770 128 MB, HD 1.82TB
- LabVIEW version (2020.01f1 64-bit)
- Arduino IDE v.2.1.0

DEPENCENCES

* AMF_SubVis  (https://amf.ch/?post_type=document)
* Fluigent SDK (https://www.fluigent.com/research/software-solutions/software-development-kit/)
* Thorlabs DSK for stepper motors (https://www.thorlabs.com/software_pages/ViewSoftwarePage.cfm?Code=Motion_Control)

INSTALLATION & RUNNING

* Install LabVIew v 2020.01f1 64-bit or later 
* Download MainLabViewProject_V2.0 directory
* Download Arduino script Stepper_plunger_Temp_Controller_26
* Launch LabView project: MicroPure_MainProject_V2.0.lvproj in MainLabViewProject_V2.0 folder
* Download and install SDK, dll libraries and SubVIs according to the equipment manufacturer's instructions
* Upload Stepper_plunger_Temp_Controller_26 to Arduino Uno using Arduino IDE 
* Launch Lab_view_applications/Version_2.0_1-Column-chip.vi for 1-column MISO chip run or 
Lab_view_applications/Version_4.9.8_2-Column-chip.vi for 2-column MISO chip run

FEATURES

The software:
* Controls AMF syringe pumps and valves. It allows to set an automated pumping procedures with user-defined volume and pump speed. 
* Records and saves chromatograms as Voltage-Volume plot.
* Allows positioning the EM grid relative to the tip of the outlet capillaty.
* Defines plunger parameters such as plunge time and force. 
* Controls ethane temperature in the liquid ethane dewar. 
