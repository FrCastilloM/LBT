# -*- coding: utf-8 -*-
"""
Created on Fri Apr 26 23:25:52 2024

@author: Francisco
"""

from tkinter import Tk, Entry, Button, Frame, Label
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

# funcion que calcula el valor de las resistencias
def calc_resist(R_matrix):
    a = np.array([1,1,1,1]).T
    return 1/12 * (6 * R_matrix @ a - (a.T @ R_matrix @ a) * a)

# Dimensiones de la matriz
n = 4

root = Tk()
root.title("Contacts R calculator")
root.geometry("500x350")  # Ajusta el tamaño de la ventana

# creo fuente del texto
bold_font = ("Arial", 12, "bold")

show=Label(root,text="")
show.pack()
show=Label(root, text="Calculadora de resistencia de contactos", font = bold_font)
show.pack()
show=Label(root,text="_______________________")
show.pack()

# Frame contenedor para organizar los elementos
frame = Frame(root)
frame.pack(expand=True, fill='both', padx=10, pady=10)

# Imagen con ecuación LaTeX
fig, ax = plt.subplots(figsize = (3,2))

fig.set_facecolor('#f0f0f0')

ax.text(0, .8, r'       [$R_{11}$,$R_{12}$,$R_{13}$,$R_{14}$]', fontsize = 14)
ax.text(0, .6, r'$\mathbf{R}$ = [$R_{21}$,$R_{22}$,$R_{23}$,$R_{24}$]   = ', fontsize = 14)
ax.text(0, .4, r'       [$R_{31}$,$R_{32}$,$R_{33}$,$R_{34}$]', fontsize = 14)
ax.text(0, .2, r'       [$R_{41}$,$R_{42}$,$R_{43}$,$R_{44}$]', fontsize = 14)
ax.axis('off')

canvas = FigureCanvasTkAgg(fig, master=frame)
canvas.draw()
canvas.get_tk_widget().grid(row=0, column=1, rowspan=4)

# Crear una matriz de entradas
entries = []

for i in range(n):
    row_entries = []
    for j in range(n):
        entry = Entry(frame, width=5)
        
        if j < i:  # Configura la entrada simétrica en la parte inferior de la diagonal
            entry.insert(0, 'R{i}{j}'.format(i = i+1, j = j+1))  # Establece los valores predeterminados en 0
            entry.config(state='readonly')  # Hace que la entrada sea de solo lectura
            
        if j == i:  # Configura la diagonal
            entry.insert(0, "0")  # Establece los valores predeterminados en 0
            entry.config(state='readonly')  # Hace que la entrada sea de solo lectura
              
        entry.grid(row=i, column=j+2)  # Ajusta la columna para dejar espacio para el texto a la izquierda
        row_entries.append(entry)
    entries.append(row_entries)
    
# entries.pack()

l1 = Label(root)
l2 = Label(root)
l3 = Label(root)
l4 = Label(root)

labels = [l1, l2, l3, l4]

def callback():
    
    R_matrix = np.zeros((n, n))
    for i in range(n):
        for j in range(n):
            if j > i:
                entry = entries[i][j]
                R_matrix[i,j] = float(entry.get())
    R_matrix += R_matrix.T
        
    resultado = calc_resist(R_matrix)
    
    for i in range(len(labels)):
        labels[i].config(text = 'R{} = %s'.format(i+1) % round(resultado[i],2))

b = Button(root, text="Calcular", command=callback)

show2=Label(root, text="by Francisco Castillo M.")
show2.place(relx=0,rely=1,anchor='sw')

for widget in (l1, l2, l3, l4, b):
    widget.pack()
    
b.mainloop()
