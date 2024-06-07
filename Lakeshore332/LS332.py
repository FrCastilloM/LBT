# -*- coding: utf-8 -*-
"""
Created on Fri Jun  7 16:29:01 2024

@author: LBT submarino
"""

import pyvisa as visa
import re
from tkinter import filedialog

def findall(textline):
    x = re.findall(r'-?\d+\.?\d*',textline)
    X = []
    for k in range(len(x)):
        X.append(float(x[k]))
    return X


def read_input():
    
    file = filedialog.askopenfilename()
    
    # abro el archivo con la tabla de datos
    input_file = open(file,'r')
    data = input_file.readlines()
    input_file.close()
    
    # creo arrays para data de la tabla
    index = []
    units = []
    temp = []
    
    # leo la tabla
    for i in range(len(data)):
        X = findall(data[i])
        if len(X) == 3:
            index.append(X[0])
            units.append(X[1])
            temp.append(X[2])
        
    return index, units, temp

class LS332:
    
    """Clase para el manejo de 332 Temperature Controller usando PyVISA de interfaz
    """
    
    def __init__(self, name):
        self._ls332 = visa.ResourceManager().open_resource(name)
        print(self._ls332.query("*IDN?"))

        # Bloquea el control manual
        self._ls332.write("LOC")
    	
    def __del__(self):
        self._ls332.close()			

    def unlock(self):
         # Desbloquea el control
        self._ls332.write("UNLOC")
        
    def get_curves(self):
        # Reviso curvas asignadas a cada termómetro
        print('')
        print('--------|Curvas asignadas a los termometros|--------')
        print('')
        print('Termometro A: ', self._ls332.query("INCRV? A"))
        print('Termometro B: ', self._ls332.query("INCRV? B"))
        print('-----------------------------------------------------')
    
    def get_memory(self):
        # Reviso espacios de memoria libres/ocupados

        for n in range(21,42):
            print("Curva {n:d}".format(n = n))
            print(self._ls332.query( "CRVHDR? {n:d}".format(n = n)))
    
    def upload_curve(self, curve, name, SN, data_format, limit, coef):
        # Reviso que la curva no este usada
        if not re.search('User Curve', self._ls332.query(f'CRVHDR? {curve}')):
            print('Slot en uso.')
            return
        
        # Tomo los datos del archivo
        index, units, temp = read_input()
        
        # Cargo la data del header en una nueva curva
        self._ls332.write(f'CRVHDR {curve}, {name}, {SN}, {data_format}, {limit}, {coef}')
        
        # Cargo la tabla de calibracion
        for i in range(len(index)):
            self._ls332.write('CRVPT {curve}, {index}, {units}, {temp}'.format(curve = curve, 
                               index = index[i], units = units[i], temp = temp[i]))
    
    def get_curve_data(self, curve):
        # Obtengo la tabla de calibracion de una curva dada
        for i in range(200):
            print(i+1, self._ls332.query('CRVPT? {curve} {index}'.format(curve = curve, index = i+1)))
        
        