# os_project_1
os_project_1<br /> 
FIFO 霍 <br />
RR  張育堂 <br />
PSJF 連+ 東逸 <br />
SJF 翁 <br />
讀檔+printf前後統整 +report: Natasha <br />
<br />
定期開會時間：禮拜三下午12:10~13:10 新生102 <br />
<br />
<br />
```
安裝syscall教學
https://medium.com/anubhav-shrimal/adding-a-hello-world-system-call-to-linux-kernel-dad32875872
我上傳的檔案裡面包含上面文章裡面3, 5, 6的部分
4的話～要自己改
每一個檔案都有對應的檔案夾，不要放錯檔案夾

改完核心之後.....
compile的方法：
第一次要compile核心和模組
sudo make -j4 用四個CPU跑(同時編譯核心和模組)
sudo make install 安裝核心
sudo make modules_install 安裝模組
reboot 重開機

之後如果再改核心的話，就不用重新裝模組了！只要裝核心就好
所以只要
sudo make bzImage
sudo make install
reboot
就好！不要sudo make modules_install
```

```
How to compile main

gcc -c *.c
gcc *.o -o main

Be careful about redefinition of same function (compare or assign_cpu ... )
If header file need to use the function defined in other header, please inlcude the header file in both .h and .c
```
```
syscall的用法

fork出新的process後,加上以下程式
struct timespec start, end;
syscall(333, &start);

process完成後，加上以下程式
syscall(333, &end);
char info[256];
sprintf(info, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start.tv_sec, start.tv_nsec, end.tv_sec, end.tv_nsec);
syscall(334, info);
```
```
1556530662	1556530665	0.00502494 
1556530665	1556530667	0.00493632
1556530667	1556530670	0.00502468
1556530670	1556530672	0.0048914
1556530672	1556530674	0.0048939
1556531093	1556531096	0.00488624
1556531098	1556531101	0.00488764
1556531103	1556531106	0.00488342
1556531108	1556531110	0.00485102
1556531113	1556531115	0.00484356
1556531118	1556531120	0.00489302
1556531123	1556531125	0.00486464
1556531127	1556531130	0.00482476
1556531132	1556531135	0.00487972
1556531137	1556531140	0.00487402
		     平均單位時間 0.004897285
```

Report要寫的東西<br />
##設計
```
資料讀進來之後，processes先依照ready time排序，固定把parent process指定給0號CPU，child processes固定給1號CPU
一旦時間數到process的ready time，就fork一個child process開始執行，parent process每經過一單位的時間，都要檢查process有沒有ready和可不可以執行。
每一單位時間都會看有沒有需要做context switch，換另一個process。

以下是SJF的部分：
看process的execution time，如果有很多個processes都已經ready，execution tim短的會先執行。假如大家execution time都相同，則用FIFO。
下一個要執行的人用sched_setscheduler把他的priority排到最優先。剛生成的child process則是先使其idle。

```
##執行範例測資的結果
```

SJF_5.txt



```

##比較實際結果與理論結果，並解釋造成差異的原因
```
實際結果會比理論結果高，可能是因為有context switch, I/O等
實際值計算方法，用kernel message的 (end time-start time)/0.004897285

測試：
SJF_5.txt 
SJF
4
P1 0    2000
P2 500  500
P3 1000 500
P4 1500 500

測試結果：
P1 5499
P2 5500
P3 5501
P4 5502

Dmesg:
[34047.512346] [project1] 5499 1556589563.967873434 1556589574.038733806

[34050.049432] [project1] 5500 1556589574.039078366 1556589576.577011311

[34052.566048] [project1] 5501 1556589574.038975110 1556589579.094966313

[34055.084470] [project1] 5502 1556589579.095186588 1556589581.614647984


理論分析：
            ready time(Start Time)	execution time	Finish Time	Finish Time-Start Time             
P1                 0                        2000            2000               2000
P2                 500                       500            2500               2000
P3                1000                       500            3000               2000 
P4                1500                       500            3500               2000
理論排程	P1(0-2000)-P2(2000-2500)-P3(2500-3000)-P4(3000-3500)
實際結果
          Start Time	    Finish Time	         Finish Time-Start Time	    (Finish Time-Start Time)/ Unit Time
P1 1556589563.967873434 1556589574.038733806              10.07085991                       2056.416815 
P2 1556589574.039078366 1556589576.577011311              2.537940025                       518.2340526
P3 1556589574.038975110 1556589579.094966313              5.055989981                       1032.406657 
P4 1556589579.095186588 1556589581.614647984               2.519459963                      514.4605207
-->P2, P3, P4 ready時，P1還在執行，所以P2, P3, P4都被block住，等到P1執行完畢之後，P3被生出來，時間被記到之後才被block，故P3時間看起來會比較長。但P3實際執行的時間還是一樣的。紀錄時間的時間點和生成child process的時間點不一樣的結果。
```

##各組員的貢獻



##PSJF
```
設計：當readytime等於系統時間則fork一個新的child process至cpu1，並在每次系統時間增加時，選擇一個execution time最低的作為下次執行的process												
												
												
												
測試：												
PSJF3.txt												
PSJF												
4		Dmesg:										
P1 0 2000		[39760.455927] [project1] 5918 1556595265.163758384 1556595267.6312500										
P2 500 500		[39785.311468] [project1] 5920 1556595289.986332922 1556595292.4583700										
P3 1000 500		[39810.143954] [project1] 5923 1556595314.854970037 1556595317.3246400										
P4 1500 500		[39883.975830] [project1] 5915 1556595240.269689428 1556595257.5847600										
												
分析：												
理論分析:												
												
	ready time(Start Time)	execution time	Finish Time	Finish Time-Start Time								
P1	0	2000	3500	3500								
P2	500	500	1000	500								
P3	1000	500	1500	500								
P4	1500	500	2000	500								
理論排程	P1(0-500)-P2(500-1000)-P3(1000-1500)-P4(1500-2000)-P1(2000-3500)											
實際結果：												
	Start Time	Finish Time	Finish Time-Start Time	(Finish Time-Start Time)/ Unit Time								
5918(P2)	1556595265.1637500	1556595267.6312500	2.4675050	503.8516195								
5920(P3)	1556595289.9863300	1556595292.4583700	2.4720440	504.7784622								
5923(P4)	1556595314.8549700	1556595317.3246400	2.4696710	504.2939120								
5915(P1)	1556595240.2696800	1556595257.5847600	17.3150799	3535.6488190								
												
平均error:	12.14320318											
```												
												
