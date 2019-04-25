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

Report要寫的東西<br />
##設計

##執行範例測資的結果

##比較實際結果與理論結果，並解釋造成差異的原因

##各組員的貢獻
