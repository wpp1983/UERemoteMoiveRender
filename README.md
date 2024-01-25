This is a UE plugin. 

# 功能流程
1. 启动web服务，等待指令
2. 接受到一个指令，记录
3. 同时只能有一个指令运行
4. 指令开始
   1. 新建等待的mp4文件， XXX_waiting.mp4
   2. 通知SequenceMaker制作Sequence
   3. 制作完成，调用 MovieRenderPipeline开始渲染Sequence 
      1. 导出 JPG 和wav，
      2. 调用FFmpeg合成mp4
   4. mp4生成完成，copy到指定的nginx目录，以供下载
   5. 删除 XXX_waiting.mp4
5. 指令结束
6. 开始下一个指令

# Install
## 开启插件
1. 开启 RMP(本) 插件
2. 开启 Remote Control API 插件
3. 开启 Movie Render Queue 插件
## 项目设置
### RMP 插件
1. Test: 设置FakeSequence，用于测试
2. Movie Pipeline Primary config：用于Render Movie的设置，下面会有详细介绍
3. Movie Output Path： Movie 的输出路径
   - RMP_MoviePipelinePrimaryConfig中也要设置一样的地址！！！
### Remote Control  插件
1. 设置端口 Remote Control HTTP Server Port
### Movie Pipeline CLI Encoder 插件 设置
主要是设置FFmpeg的路径和一些默认参数，参考 [UE官方文档](https://docs.unrealengine.com/5.3/en-US/cinematic-rendering-export-formats-in-unreal-engine/)

# StartUP
## 启动Web服务
启动UE editor， 在命令行输入
```
WebControl.StartServer
```
## 打开默认场景
- 打开Plugins\RemoteMovieRender\Content\Starter.ump
- 场景中的WebListener 会接受web消息

# WebAPI
### 查询当前web是否已经启动
- GET remote/info
- 例子：http://127.0.0.1:30010/remote/info

### 创建Movie命令
- PUT remote/object/call
- 例子：http://127.0.0.1:30010/remote/object/call
``` json
{
  "objectPath" : "/RemoteMovieRender/Starter.Starter:PersistentLevel.RMP_WebListener_0",
  "functionName" : "OnNewMovieRequest",
  "parameters" : 
  {
    "InMovieName" : "TestName",
    "InLLMParam" : "Test",
  },
  "generateTransaction" : true
 }
 ```
### 创建Movie(使用假的sequence)命令 
和上面一样，只是把函数名改一下
```
  "functionName" : "OnNewMovieRequestFakeSequence",
 ```
# Movie Render 设置
- 设置文件是：Plugins\RemoteMovieRender\Content\RMP_MoviePipelinePrimaryConfig
- 具体设置请参考官方文档