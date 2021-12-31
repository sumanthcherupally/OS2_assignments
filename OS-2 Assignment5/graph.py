import matplotlib.pyplot as plt

avg_times=[0.053,0.075,0.088,0.094,0.106]
worst=[0.223,0.302,0.345,0.380,0.359]
n=[2,5,10,15,20]

plt.plot(n,avg_times,label="Average wait Time")
plt.plot(n,worst,label="Worst Case wait Time")
plt.legend(loc='upper left')
plt.title("Avg and worst case waiting times vs No of philosopher threads")
plt.xlabel("Time(s)")
plt.ylabel("No of philosopher threads")
plt.grid()
plt.show()