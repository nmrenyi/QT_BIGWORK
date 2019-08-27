# DMFB SIMULATOR 设计文档
## 计86任一 2018011423

### 一、结构简介
- main.cpp是程序的入口
- 设计有两个窗体，分别对应mainwindow.ui和dialog.ui
- command.h以及command.cpp主要负责构建Command类，Command类负责记录从文件中读取的命令名称以及命令相关参数，方便后续调用。
- square.h以及square.cpp主要负责构建Square类和Droplet类。Square类代表芯片的每一个格子，记录着该格子在何时刻拥有何液滴等重要属性，Droplet类代表相应的液滴，记录着颜色、形状等重要属性。
- mainwindow.h和mainwindow.cpp主要负责mainwindow窗体的构建和相关重要逻辑代码。
- dialog.h和dialog.cpp主要负责dialog窗体的构建和相关重要逻辑代码。

### 二、UI设计
- mainwindow.ui中的界面主要描述了程序运行的主体窗口，分别包括以下按钮：NewSession按钮负责弹出窗口输入芯片的行数和列数等，ReadFile按钮主要负责弹出窗口选择所需要的指令文档，NextStep和PreviousStep主要负责下一步和上一步，PlayAll负责进行播放全部，Reset负责将界面返回到零时刻。
- dialog.ui中的界面主要描述了点击NewSession按钮后弹出输入芯片信息的窗口。
- 在进行UI设计时，我尝试了直接使用QT Designer和手动代码编写UI界面两种方式，此外还对界面进行了适当的美化，例如加入Spacer，使用布局等等方式。

### 三、主要实现方法
- 本项目主要采用mainwindow.h当中的Square status三元组(时间、行、列)来记录每个时刻在芯片每一行每一列的状态，从而实现播放、重置、清洗等功能。
- 指令分析：在文件读取后，对于输入的命令进行分析，将每一时刻的命令通过计算转化转换为每一时刻每一个格子的状态，记录到上面的status三元组当中。
- 进行播放：播放时遍历每一时刻，在每一时刻中遍历所有格子，根据每一个格子的状态进行相应操作(例如画液滴和污染标注)，直到最终所有时刻均已遍历。