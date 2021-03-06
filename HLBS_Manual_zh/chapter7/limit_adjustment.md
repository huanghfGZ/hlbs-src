# 门限调整

* 错误率计算方法：

* 默认错误数 / (错误数+成功数)，在错误数与成功数有问题的情况下（这两个值是由客户端上报的，可能有问题），为错误数 / 总请求数。

```
    1. 总请求数：调用Agent获取路由接口次数。
    2. 理论上总请求数，不会低于错误数、超时数、成功数之和，低于的话强制重置为三者之和，高于的话，直接使用。这使得总请求数与上报次数无关。
```

* 拒绝率计算：拒绝数 / 总请求数。

* 平均错误率计算方法：

```
    在非压力故障的情况下，统计多个周期的错误率并加以平均得到。
```

* 有效错误率计算方法：

```
    实际错误率 - 平均错误率。
```

* 当有效错误率低于配置文件中所定义的最低错误率时，

```
1. 在过载时（拒绝数 > 0）

    若存在过度扩张（门限增加后错误率超过预定义的最小错误率），则：
        若本周期成功请求数超过上次过度扩张时的阀值，则可按照配置文件中所定义的扩张因子进行扩张；
        否则，扩张值为（上次过度扩张时的阀值-本周期请求数）*扩张因子；最小扩张值为配置文件中定义的req_min；
    若不存在过度扩张，则扩张值为 本周期阀值 * 扩张因子 / delay_load

2. 不存在过载时，如果总请求数大于门限（这是可能的，在前面对预取数的计算过程中提到，如果错误数为0，则可以分配高于门限的请求），将门限设为请求数；

3. 如果门限大于预定义的req_max，设置门限为req_max。

当有效错误率高于配置文件中所定义的最低错误率时，收缩门限值，收缩值为错误率 * 门限。并且，如果请求数高于req_min，则表明存在过度扩张，需要记录这个门限值作后续调整的参考。
门限收缩的下限是req_min。当收缩至req_min表明该Svr过载（初始化为req_min+1），并不再被选中，知道探测线程成功并达到恢复条件为止。
```
