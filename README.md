# OS project in NTUST

## 🙂 使用情境說明(包含流程圖)

### Lazy Allocation Memory
正常情況下process都是先申請Memory，然後再使用。但是有一個process有的時候會一次性申請非常多的Memory，但是可能又不會用到，比如說多餘的array空間。

所以就可以進行一些優化，當process申請的時候不給其分配Memory，直到其使用這個Memory的時候才allocate memory 給這個process。

實作Lazy Allocation Memory主要分為兩個部分：申請Memory的時候不實際分配Memory、使用的時候Allocate memory

#### 申請Memory的時候不實際分配Memory

當process申請Memory的時候只是增加process的Memory大小指標，但是不給其分配實際的memory：

原來process申請Memory的流程：

![](https://i.imgur.com/5jxdVMq.png)

修改以後，只增加process的size而不實際分配Memory：

![](https://i.imgur.com/XNQG8R2.png)

#### 使用的時候Allocate memory

當process真正需要使用Memory的時候回因為找不到address而發生page fault，這個時候再allocate memory：

![](https://i.imgur.com/h3sSrqO.png)

---

### Scheduling:Multilevel feedback queue with priorit
通過proc.c中的scheduler部份我們可以看出xv6採用的是Round Robin算法來進行排程，由於這個排程演算法非常簡單，缺乏靈活性，因此我們修改了scheduler部份，實現一個帶priority的multilevel feedback queue，在每個隊列中採用優先級高的程序先執行的算法，在各個隊列之間優先級越低的隊列有越多的執行時間。這樣我們可以將系統程序的優先級提高，也可以手動修改程序的priority從而指定某些程序優先執行。
#### 基本設置


| queue | priority | runtime |
| -------- | -------- | -------- |
| 1     | 0-4     | 1/100sec     |
|2|5-14|5sec|
|3|15-20|10sec|

#### 增加systemcall以及用戶程序說明


| syscall | 功能 | 
| -------- | -------- | 
| cps    | 打印process的name、pid、state、priority、rrnum（目前的運行次數）  | 
|chpr|修改程序的priority|

| 用戶程序 | 功能 |
|-------|-------|
|ps|調用cps系統調用|
|mkprc|test用的程序，會fork，然後進行循環的運算以消耗時間，便於排程算法結果顯示|
|chpr|檢測輸入priority是否合法，然後調用chpr系統調用|


#### 演算法說明
第一隊列中包含priority為0-3的程序，第二隊列中包含priority為4-14的程序，第三隊列中包含priority為15-20的程序。

第一隊列中的程序每次分到一個時間片，即1/100sec，除非手動調整否則其priority不會發生改變。
第二隊列中的程序每次分到500（為了便於顯示效果，此處將數據設定得比較大）個時間片，即5sec。為了避免starvation，執行5sec後該程序的priority就會+1，當其priority達到15，即進入第三隊列。若有新的程序產生且其優先級高於它，則會轉去執行優先級更高的程序。
第三隊列中的程序每次分到1000個時間片，即10sec。每執行1000個時間片，priority即+1，直到其priority升到20為止。

對於init和sh我們分配了最高的優先級0，其他從shell中執行的程序我們會分配優先級為4，都屬於第一隊列；對於其他程序，像fork出來的子程序，默認優先級為5。

#### 流程圖
##### 系統運行流程圖
實際上運行起來之後就是在scheduler中的swtch(&(c->scheduler),p->context)和shed中的swtch(&p->context,mucpu()->scheduler)這兩行代碼之間來回切換
![](https://i.imgur.com/z8HwZrW.png)
##### scheduler部份，原始流程圖![](https://i.imgur.com/NxB1Nao.png)
##### scheduler部份，修改後的流程圖![](https://i.imgur.com/NfFs8M3.png)



## 😇 成功畫面

### Lazy Allocation Memory

![](https://i.imgur.com/9zzyaMb.png)

### Scheduling:Multilevel feedback queue with priorit
#### default priority
初始時，可以看到init和sh的priority最高，從shell執行的ps的priority為4
![](https://i.imgur.com/GSVyYDN.png)
從shell執行的mkprc默認priority也是4，但是fork出的子程序默認priority為5
![](https://i.imgur.com/cO4XWv3.png)
#### 第二隊列（priority：5～14）priority增加情況
可以看到當7（pid）運行了500個時間片後，他的優先級就會由5上升到6，時間片重新開始計數
![](https://i.imgur.com/ZBdleUV.png)
#### 第三隊列（priority：15～20）priority增加情況
在執行了1000個時間片之後，11的優先級由15變成了16
![](https://i.imgur.com/K4mE5Ur.png)

#### 優先級高的process會優先執行
可以看到，由於18的優先級較7要高，所以一直在執行18，7的priority(11)和rrnum(312)都沒有發生變化，說明它並沒有得到執行，且18每執行500個時間片後，
![](https://i.imgur.com/r1kQNm7.png)

#### 修改程序的優先級
用chpr程序將6的priority由10變為6（紅色部分）；藍色部份可以看到6的優先級提高後將一直執行優先級較高的6。
![](https://i.imgur.com/NnPnFqm.png)

## 更多
更多xv6项目可以参考[这里](https://ntust-csie-islab.github.io/106-OS-homework2-2/)
