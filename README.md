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
1556530662	1556530665	0.00502494 <br />
1556530665	1556530667	0.00493632<br />
1556530667	1556530670	0.00502468<br />
1556530670	1556530672	0.0048914<br />
1556530672	1556530674	0.0048939<br />
1556531093	1556531096	0.00488624<br />
1556531098	1556531101	0.00488764<br />
1556531103	1556531106	0.00488342<br />
1556531108	1556531110	0.00485102<br />
1556531113	1556531115	0.00484356<br />
1556531118	1556531120	0.00489302<br />
1556531123	1556531125	0.00486464<br />
1556531127	1556531130	0.00482476<br />
1556531132	1556531135	0.00487972<br />
1556531137	1556531140	0.00487402<br />
		         平均單位時間 0.004897285<br />

Report要寫的東西<br />
##設計

##執行範例測資的結果

##比較實際結果與理論結果，並解釋造成差異的原因

##各組員的貢獻
