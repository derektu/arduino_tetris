# Arduino for Tetris

- http://github.com/derektu/arduino_tetris

## 步驟

- 初始化藍芽device, 同時測試藍芽通訊
- 測試Colorduino的color display
- 測試Arduion跟單台Colorduino的通訊
- 測試Arduion跟兩台Colorduino的通訊

## 藍芽setup

- [教學影片](https://www.youtube.com/watch?v=fkS1elBSzgs)
- 線路
    - arduino 5v -> hc-05 5v
    - arduino gnd -> hc-05 gnd
    - arduino pin 10 -> hc-05 tx
    - arduino pin 11 -> hc-05 rx

- 拔掉 5v
- 上傳 bt_setup_hc_05.ino
- 按著hc-05上面的reset button, 然後connect 5v的線路, 這時候hc-05的紅燈會開始慢慢閃爍, 進入AT mode
- 打開IDE的serial monitor, baudrate設定成9600, CR+LF
- [AT指定方式](http://ruten-proteus.blogspot.tw/2014/07/Bluetooth-Kit-tutorial-02-hc-05-02.html)
    - 輸入 AT <enter>, 應該可以看到OK
    - 設定baudrate
        - AT+UART?
        - AT+UART=115200,0,0
    - 設定password
        - AT+PSWD?
        - AT+PSWD=0000
    - 設定名稱
        - AT+NAME?
        - AT+NAME=tetris

- 設定完成之後, 拔掉5v, 重新接回, 這時候hc-05會回到接收模式
- 從mac端試著連線到藍芽模組
- 上傳 bt_test.ino
- [mac端打開CoolTerm](http://swf.com.tw/?p=499)
    - 點Option, SerialPort 選擇連線到 tetris-DevB, Baudrate選擇115200, Terminal的部分選擇Line Mode, CR+LF
    - 從CoolTerm內輸入"1", 看arduino的LED燈有沒有亮, 輸入"0", 看LED燈是否滅掉

## 測試Colorduino的color display

- Colorduino是一個類似Arduino的device, 上面需要執行一個獨立的程式, 可是Colorduino不像Arduino, 可以直接從pc的usb
  上傳程式, 需要透過[FTDI FT232RL模組](http://goods.ruten.com.tw/item/show?21110312965371)
- 由於買到的這個FTDI模組的晶片是大陸改過的CH-340, 在Mac上面需要另外安裝driver
    - http://arduino.stackexchange.com/questions/12133/mac-osx-yosemite-no-serial-ports-showing-for-uno-r3
    - http://www.wch.cn/download/CH341SER_MAC_ZIP.html
- FTDI與Colorduino的接線方式
    - FTDI DTR -> colorduino DTR
    - FTDI GND -> colorduino DTR
    - FTDI VCC -> colorduino VDD
    - FTDI TXD -> colorduino RXD
    - FTDI RXD -> colorduino DXD
- Mac端的Arduino需安裝Colorduino的library
    - copy libraries/Colorduino to Arduino的library目錄
- upload cduo_plasma程式, 測試Colorduino的顯示是否正常
    - Board: Arduino Duemilanove or Diecimila
    - Port: /dev/cu.wchusbserial1410
- upload cduo_colortest程式, 測試Colorduino的顯示是否正常

## 串連 Arduino and Colorduino

- 由於Colorduino只能顯示畫面, 所以執行程式邏輯的Arduino必須把要顯示的資訊傳給Colorduino來顯示,
- 傳送資料的方式是透過I2C protocol (除了可以串連Arduino跟Colorduino之外, 也可以串連Arduino to Arduino, 或是其他支援I2C的devices)
    - https://www.arduino.cc/en/reference/wire
    - http://programmermagazine.github.io/201311/htm/article1.html
    - http://gogoprivateryan.blogspot.tw/2014/06/108-2-arduino-sg90-animatronic-eye-i2c.html

- 在I2C protocol內分成master跟slave, 一個master可以跟多個slaves串連, 每一個slave需要有一個獨立的address (介於8跟127之間)

- upload cduo_slave程式 to Colorduino
    - 啟動時自己的I2C的device id設定為8
    - 由於Arduino的I2C protocol限制每次傳送的bufer大小為32個bytes, 所以我們把master傳給slave的資料格式定義如下
        - 格式: <START_OF_DATA> <RowIndex> <Pixel> <Pixel> .. <END_OF_DATA>
        - 代表要更新某一行(RowIndex)的資料, 每一個Pixel是一個byte, 代表這個位置要顯示的數值(0, 1, 2, 3, ..),
          也就是Tetris的block分類 (在cdo_slave程式內另外定義了一個Palette table, 表示每一個數值所要對應的顏色)
        - cdo_slave內必須保留完整的畫面大小(8 x 8), 每次收到時只更新這個row的資料 (可是要display到Colorduino時必須
          把整個畫面都傳出去)

- upload aduo_master程式 to Arduino
    - 這個程式會透過I2C把要顯示的畫面傳給device #8

- 串連Arduino & Colorduino
    - Arduino pin A5 <-> Colorduino SCL
    - Arduino pin A4 <-> Colorduino SDA
    - Arduino 5v <-> Colorduino VDD
    - Arduino gnd <-> Colorduino gnd
    - 請參考[接線圖](http://gogoprivateryan.blogspot.tw/2014/06/108-2-arduino-sg90-animatronic-eye-i2c.html)
        - 必須在5V -> Analog pin 以及 SDA/SCL 之間串接一個4.7k Om的電阻
    - 必須使用外接電源, 否則LED的顯示會不正常

- 1對2的測試
    - 上傳cduo_slave to Colorduino#8, Colorduion#9 (第一台的device id=8, 第二台的device id= 9)
    - 上傳aduo_master_2 to Arduino
    - 接線方式跟單台的一樣, 兩台Colorduino之間用內建的機制串連

## TODO

- 支援藍芽4.0 (iPhone)

