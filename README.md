# arduino-air-monitor

DIY空气质量仪

## 介绍

[CSDN](https://blog.csdn.net/xachary2/category_12867389.html) | [掘金](https://juejin.cn/column/7454100785795006515)

## 开发板

Arduino Nano（国产）

## 传感器

- DART WZ-S 甲醛（HCHO）
- SGP40 总挥发物（TVOC）
- MTP40-F 二氧化碳（CO2）
- DHT22 温湿度
- GUVA-S12SD 紫外线（UV）

## 其它模块

- DS1302 时钟
- SSD1306 OLED显示器

## 指标参考说明

- HCHO：
80ug/m3（即国家室内标准0.08mg/m3）

- TVOC(指数)：
优秀[0～100]、递增[100～500]

- CO2(ppm)：
室外[350～450]、清新[350～1000]、浑浊[1000～2000]、头痛[2000～5000]、缺氧[‌>5000]

- UV(指数)：
白0[<50]、绿1[227]、绿2[318]、黄3[408]、黄4[503]、黄5[606]、橙6[696]、橙7[795]、红8[881]、红9[976]、红10[1079]、紫11[1170+]

## 图片

![image](https://github.com/user-attachments/assets/c666f129-c04d-48cf-b6ed-5956a903378c)

![image](https://github.com/user-attachments/assets/c3da1199-b0d8-4e64-9e3f-a427591af015)

![image](https://github.com/user-attachments/assets/4e3e710c-7d12-4e2d-9d13-4cca5e9cd81b)
