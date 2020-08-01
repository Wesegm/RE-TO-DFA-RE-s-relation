# RE-TO-DFA-RE-s-relation
Regular Expression to NFA  DFA &amp; MinDFA  and computing relations of two RE

#how to run:
```g++ -std=c++11 re_dfa.cpp```
```./a.exe```
or in linux
```./a.out```

# input
num of cases

re1 re2

...
# ouput
relations of two re

# example
in:

```
1
a*b aab
```
out:

```
<
```

## 正则表达式 --> NFA -->DFA --> MIN_DFA
## 也可以对两个正则表达式之间的比较
## 关系有： 包含 包含于 等于 没有包含关系
