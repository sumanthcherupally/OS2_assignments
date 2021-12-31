import matplotlib.pyplot as plt
import numpy as np

CAS=[0.37,0.82,1.12,1.54,1.95,2.61]
CAS_worst = [3.16,5.7,9.6,13.8,17.5,24.3]

TAS=[0.43,1.11,1.67,2.2,2.83,3.59]
TAS_worst = [4.3,9.3,15.4,20.3,29.4,39.3]

BW=[0.86,1.8,3.1,4.2,5.4,7.1]
BW_worst=[2.68,5.32,7.8,11.5,15.2,21.2]

p=[50,100,150,200,250,300]

plt.plot(p,CAS_worst,label = 'CAS')
plt.plot(p,TAS_worst,label='TAS')
plt.plot(p,BW_worst,label='CAS - bounded')
plt.ylabel('Worst waiting time(sec)')
plt.xlabel('No. of Processes(10 CS each)')
plt.legend(loc='upper left')
plt.title("Worst waiting time")
plt.grid()
plt.show()