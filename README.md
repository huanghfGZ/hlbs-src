#简介
   该系统致力于为CGI在使用某一（GID、XID）Svr时，提供负载均衡与容错保护。
   为实现该目标，系统为所有Svr实现按类别（GID、XID）的动态负载均衡和过载保护功能。

#目标
1）动态负载均衡：
   通过外部观测每一Svr的主机信息，包括CPU、网络流量、内部统计等指标，通过某一算法转换成0-100之间的数值表示该Svr处理能力，这个值将作为CGI对Svr的访问依据。简单的讲，就是让主机性能也作为能影响负载均衡的一个因素。

2）过载保护：
   使用时间片（不要低于CGI调用AgentSvrd最小相邻时间间隔的2倍，建议5~60s。）内的访问作为统计单位，时间片内所有访问平均延时、成功率等信息作为下一个时间片内请求参照。即通过收集x时间片Svr响应结果成功率、延时信息判断Svr在x+1时间片对CGI的处理质量，是否适合继续服务。当响应结果失败率增大/减小时，降低/提高访问量来减轻/增加该Svr服务压力。
采用时间片方式进行Svr统筹分配，若值(时间片、阈值)设置得当，不管是Svr处理能力下降、上升，还是宕机（过载或非压力故障），系统都能及时检测并调整权重来逐步应对机器性能在物理上是线性变化的事实。被判定为宕机（过载或非压力故障）的Svr将会从候选路由列表中被删除，并被加至宕机列表由探测线程定时探测该Svr以待恢复。

3）集中配置
   通过更新RouterSvr下svr.xml配置文件，集中进行管理所有后端Svr。

#详情
   见目录：[用户手册](HLBS_Manual_zh/README.md)
