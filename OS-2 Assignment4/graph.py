import matplotlib.pyplot as plt
import numpy as np


q1_avg_reader_rw=[0.1023,0.0426,0.0081,0.0088,0.00807,0.0077,0.0075,0.0072,0.0066]
q1_avg_writer_rw=[0.1026,0.1064,0.1032,0.1047,0.1071,0.11102,0.1084,0.1062,0.1067]
q3_worst_reader_rw=[0.1420,0.1248,0.1124,0.1205,0.1247,0.1208,0.1242,0.12163,0.1183]
q3_worst_writer_rw=[0.147,0.2429,0.3342,0.3505,0.3719,0.3828,0.3909,0.3770,0.3867]

q1_avg_reader_f=[0.10135,0.12178,0.12963,0.14282,0.15149,0.16654,0.16651,0.16291,0.18575]
q1_avg_writer_f=[0.1021,0.1213,0.1280,0.1408,0.1490,0.1617,0.162852,0.1598,0.1819]
q3_worst_reader_f=[0.16794,0.24336,0.26275,0.19027,0.22263,0.24046,0.30038,0.21151,0.26251]
q3_worst_writer_f=[0.14151,0.21896,0.15665,0.22510,0.21631,0.25091,0.23772,0.21969,0.32697]

q2_avg_reader_rw=[0.00071,0.00194,0.00316,0.00413,0.00811,0.00872,0.00881,0.0108,0.0134]
q2_avg_writer_rw=[0.0128,0.028,0.052,0.079,0.107,0.132,0.157,0.180,0.207]
q4_worst_reader_rw=[0.0101,0.0350,0.0617,0.0643,0.118,0.154,0.165,0.182,0.214]
q4_worst_writer_rw=[0.2424,0.2708,0.313,0.332,0.368,0.382,0.403,0.407,0.440]

q2_avg_reader_f=[0.01122,0.06574,0.10348,0.12650,0.14581,0.19604,0.19106,0.23249,0.25835]
q2_avg_writer_f=[0.01240,0.06566,0.10196,0.12682,0.14388,0.19484,0.18839,0.22714,0.25540]
q4_worst_reader_f=[0.02454,0.12524,0.18270,0.24132,0.26536,0.28897,0.33478,0.35810,0.38206]
q4_worst_writer_f=[0.0307,0.1561,0.1786,0.2153,0.2486,0.2967,0.3473,0.3849,0.4174]

q1_readers=[1,5,10,15,20,25,30,35,40]
q2_writers=[1,5,10,15,20,25,30,35,40]
q3_readers=[1,5,10,15,20,25,30,35,40]
q4_writers=[1,5,10,15,20,25,30,35,40]

plt.plot(q1_readers,q1_avg_reader_rw,label='reader_RW')
plt.plot(q1_readers,q1_avg_writer_rw,label='writer_RW')
plt.plot(q1_readers,q1_avg_reader_f,label='reader_fair')
plt.plot(q1_readers,q1_avg_writer_f,label='writer_fair')
plt.legend(loc='upper left')
plt.grid()
plt.xlabel("No of readers")
plt.ylabel("Average waiting time (sec)")
plt.show()

plt.plot(q2_writers,q2_avg_reader_rw,label='reader_RW')
plt.plot(q2_writers,q2_avg_writer_rw,label='writer_RW')
plt.plot(q2_writers,q2_avg_reader_f,label='reader_fair')
plt.plot(q2_writers,q2_avg_writer_f,label='writer_fair')
plt.legend(loc='upper left')
plt.grid()
plt.xlabel("No of writers")
plt.ylabel("Average waiting time (sec)")
plt.show()

plt.plot(q3_readers,q3_worst_reader_rw,label='reader_RW')
plt.plot(q3_readers,q3_worst_writer_rw,label='writer_RW')
plt.plot(q3_readers,q3_worst_reader_f,label='reader_fair')
plt.plot(q3_readers,q3_worst_writer_f,label='writer_fair')
plt.legend(loc='upper left')
plt.grid()
plt.xlabel("No of readers")
plt.ylabel("Worst time (sec)")
plt.show()

plt.plot(q4_writers,q4_worst_reader_rw,label='reader_RW')
plt.plot(q4_writers,q4_worst_writer_rw,label='writer_RW')
plt.plot(q4_writers,q4_worst_reader_f,label='reader_fair')
plt.plot(q4_writers,q4_worst_writer_f,label='writer_fair')
plt.legend(loc='upper left')
plt.grid()
plt.xlabel("No of writers")
plt.ylabel("Worst time (sec)")
plt.show()