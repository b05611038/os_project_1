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
            ready time      Start Time  	execution time	Finish Time	Finish Time-Start Time             
P1                 0          0                   2000            2000               2000
P2                 500        2000                 500            2500               500
P3                1000        2500                 500            3000               500 
P4                1500        3000                 500            3500               500
理論排程	P1(0-2000)-P2(2000-2500)-P3(2500-3000)-P4(3000-3500)
實際結果
          Start Time	    Finish Time	         Finish Time-Start Time	    (Finish Time-Start Time)/ Unit Time
P1 1556589563.967873434 1556589574.038733806              10.07085991                       2056.416815 
P2 1556589574.039078366 1556589576.577011311              2.537940025                       518.2340526
P3 1556589574.038975110 1556589579.094966313              5.055989981                       1032.406657 
P4 1556589579.095186588 1556589581.614647984               2.519459963                      514.4605207

平均error: 155.3795113
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
	          Start Time	        Finish Time	Finish Time-Start Time	(Finish Time-Start Time)/ Unit Time								
5918(P2)	1556595265.1637500	1556595267.6312500	2.4675050	503.8516195								
5920(P3)	1556595289.9863300	1556595292.4583700	2.4720440	504.7784622								
5923(P4)	1556595314.8549700	1556595317.3246400	2.4696710	504.2939120								
5915(P1)	1556595240.2696800	1556595257.5847600	17.3150799	3535.6488190								
												
平均error:	12.14320318											
```												

## RR

```
資料讀進來後，先用sort把process的ready time先排序過一次，當main開始跑後，以while loop當作每一次的unit time，每次迴圈中先檢查是否有程序的值星
時間歸0 (process finish)，之後看是否有程序需要fork進子程序序列中。因為RR可能需要context switch，所以每次loop中會先檢查下一個time step中需要>執行的process，而此函式中有儲存上一個context switch的時間，確認完後，若要context switch，就將child cpu中正在執行的process降低priority，然後>讓下一個要執行的程序提高priority，此迴圈一直進行到所有程序跑完為止。

## 執行範例測資的結果
Unit time: 0.004897285

RR_3.txt
P1      1200    5000
P2      2400    4000
P3      3600    3000
P4      4800    7000
P5      5200    6000
P6      5800    5000

Process PID
P1      2450
P2      2451
P3      2452
P4      2453
P5      2454
P6      2455

Dmesg
[ 2005.419856] [project1] 2452 1556544737.723564672 1556544800.084102358

[ 2008.537686] [project1] 2450 1556544724.884969612 1556544803.203493922

[ 2011.411529] [project1] 2451 1556544731.539740827 1556544806.078770934

[ 2051.619281] [project1] 2455 1556544747.219257127 1556544846.306630577

[ 2061.552951] [project1] 2454 1556544744.975531956 1556544856.245265956

[ 2067.272330] [project1] 2453 1556544743.583076210 1556544861.967504898

PID     Start time              End time                Actual Unit time        Ideal unit time	Start time	Finish time
2450    1556544724.884969612    1556544803.203493922    15992.23331             18500		1200		18700
2451    1556544731.539740827    1556544806.078770934    15220.48035             16800		2400		19200
2452    1556544737.723564672    1556544800.084102358    12733.69585             14600		3600		18200
2453    1556544743.583076210    1556544861.967504898    24173.48182             26400		4800		31200
2454    1556544744.975531956    1556544856.245265956    22720.69810             25000		5200		30200
2455    1556544747.219257127    1556544846.306630577    20233.12375             22400		5800		28200

平均error: -2104.381

##比較實際結果與理論結果，並解釋造成差異的原因
Unit time在冊的時候可能沒有考慮到每個人寫的main不一樣，所以造成些許時間上的差異，而當執行時間變長的時候此差異就明顯了起來。而RR在執行時時間>消耗越來越短，除了Unit time的誤差以外，可能是因為此RR演算法在設計時，每一次預測下一個time step要跑的function中，有一個要跑過所有存在程序的for loop，而當程序執行到越後面時，因為先執行完的process已經消失了，此時fpr loop要跑得iteration變少了，而且在context switch的時候也需要時間，在
程式執行到越後面要context switch的次數也越來越少，所以在長時間的執行下，這些差異就顯現出來了。
```												
