import matplotlib.pyplot as plt
import numpy as np

n=[100,90,80,70,60,50,40,30,20]
rm_miss=[68,61,55,46,36,26,17,17,10]
rm_wait=[4025,3325,3062,2437,1883,1246,918,709,209]
edf_miss=[88,80,70,58,44,34,27,22,10]
edf_wait=[4996,4185,3871,3045,2334,1795,1426,1260,670,]

plt.plot(n,rm_miss,label = 'RM')
plt.plot(n,edf_miss,label='EDF')
plt.ylabel('No of missed deadlines')
plt.xlabel('No. of Processes')
plt.legend(loc='upper left')
plt.title("No of missed deadlines")
plt.grid()
plt.show()