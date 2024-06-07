# -*- coding: utf-8 -*-
"""
Created on Wed Jun  5 13:53:09 2024

@author: LBT submarino
"""

import os
os.chdir(r'C:\Users\LBT submarino\OneDrive\Escritorio\Fran C')
from LS332 import LS332

#%% Busco nombre del controlador de temperatura

import pyvisa as visa
rm = visa.ResourceManager()
rm.list_resources()

#%% Manipulacion controlador de temperatura

# Abro comunicacion
ls332 = LS332(name = 'GPIB0::12::INSTR')

# Obtengo curvas asignadas a los termometros
ls332.get_curves()

# Reviso espacios de memoria libres/ocupados
ls332.get_memory()

#%% Cargo curva nueva

''' Archivo de entrada tipo (termometro de la muestra):
    
Sensor Model:   CX-1050-SD-1.4L
Serial Number:  X23517
Data Format:    4      (Log Ohms/Kelvin)
SetPoint Limit: 325.      (Kelvin)
Temperature coefficient:  1 (Negative)
Number of Breakpoints:   150

No.   Units      Temperature (K)

  1  1.81425       325.000

'''

name = 'CX-1050-SD-1.4L'
SN = 'X23517'
data_format = 4
limit = 325
coef = 1

ls332.upload_curve(27, name, SN, data_format, limit, coef)

#%% Resivo que se haya cargado bien

ls332.get_curve_data(27)
