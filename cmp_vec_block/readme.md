
## 在对多个字符串匹配时， vector模式并没有比BLOCK快

+ 多个字符串匹配， 轮询block模式匹配仍然好于vector模式
+ 10组数据匹配结果都是BLOCK速度快

[root@openapi cmp_vec_block]# ./block_test
=====[0] match
cost block:[2] vector:[50]
=====[1] match
cost block:[2] vector:[8]
=====[2] match
cost block:[2] vector:[4]
=====[3] match
cost block:[2] vector:[7]
=====[4] match
cost block:[1] vector:[7]
=====[5] match
cost block:[2] vector:[3]
=====[6] match
cost block:[2] vector:[3]
=====[7] match
cost block:[2] vector:[3]
=====[8] match
cost block:[1] vector:[4]
=====[9] match
cost block:[2] vector:[15]

