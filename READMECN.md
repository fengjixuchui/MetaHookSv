# MetaHookSv

MetaHook的SvenCoop移植版本 (https://github.com/nagist/metahook)

目前暂时跟原版GoldSrc不兼容，不过后续可以改进成兼容版本。

## 安装方式

1. 复制Build目录下的所有文件到 "你的Steam游戏库目录\steamapps\common\Sven Co-op\".

2. 从"你的Steam游戏库目录\steamapps\common\Sven Co-op\svencoop.exe" 启动游戏。

* Build中的 "svencoop.exe" 原来叫 "metahook.exe"，它会替换你自带的游戏启动器svencoop.exe，请注意备份。当然你也可以选择不替换“svencoop.exe”，以命令行或启动项方式"metahook.exe -game svencoop"启动SvenCoop，不过不推荐这么做，因为这么做会导致更改视频模式的时候游戏闪退（可能是游戏自己对进程名有校验）。

* Build中的“SDL2.dll”文件是用来修复原版SDL使用中文输入法进行游戏时可能发生的内存越界写入导致游戏崩溃的问题。如果你全程都关闭中文输入法的话也可以选择不替换“SDL2.dll”。

## 插件

### CaptionMod

这是一个使用VGUI2来显示字幕、翻译HUD消息的插件

原始版本：https://github.com/hzqst/CaptionMod.

#### 功能

1. 播放声音时显示字幕

2. 播放短句（sentence）时显示字幕

3. 收到HUD文字消息的时候显示字幕

4. 修改原版客户端的HUD文字消息（如友军敌人血量显示以及game_text实体显示的文本），为其添加多字节字符支持。

4. 修改VGUI1的文字控件，为其添加多字节字符支持。

5. 每张地图支持单独的自定义翻译字典, 字典文件在"/maps/[mapname]_dictionary.csv"

![](https://github.com/hzqst/MetaHookSv/raw/main/img/1.png)

### MetaRenderer

替换了原版的图形渲染引擎，使用了黑科技提升你的游戏体验。

![](https://github.com/hzqst/MetaHookSv/raw/main/img/2.png)

![](https://github.com/hzqst/MetaHookSv/raw/main/img/3.png)

![](https://github.com/hzqst/MetaHookSv/raw/main/img/4.png)

![](https://github.com/hzqst/MetaHookSv/raw/main/img/5.png)

#### Features

1. HDR后处理

2. 简单水面反射折射 (警告：开启该功能可能导致严重的性能损失，请根据掉帧严重程度自行斟酌是否开启！)

3. 简单逐对象阴影 (警告：开启该功能可能导致严重的性能损失，请根据掉帧严重程度自行斟酌是否开启！）

4. 屏幕空间遮蔽 (SSAO) (警告：开启该功能可能导致严重的性能损失，请根据掉帧严重程度自行斟酌是否开启！）

5. 多重采样抗锯齿 (MSAA)  (警告：开启该功能可能导致轻微的性能损失，但配合SSAO使用则可能严重影响帧数，请根据掉帧严重程度自行斟酌是否开启！）

6. 快速近似抗锯齿 (FXAA) （仅当MSAA不可用时）.

7. 延迟光照技术和逐像素光照渲染（仅不透明物体），支持“无限”的动态光源(SvEngine最多256个)并且几乎没有性能损失。

9. mdl模型渲染可使用顶点缓冲对象、合并DrawCall、GPU光照来进行优化，使用这些技术可以解放CPU算力，做到同屏渲染20万多边形的mdl仍能维持在可接受的帧数。

10. bsp地形渲染可使用顶点缓冲对象、合并DrawCall来进行优化，使用这些技术可以解放CPU算力，做到同屏渲染10万多边形的bsp地形仍能维持在可接受的帧数。

#### 计划实现

1. 模型印花

2. 布娃娃引擎

3. 粒子系统（可能不）

#### 启动项参数

-nofbo : 禁用帧缓冲对象. SSAO和延迟光照将不可用.

-nomsaa : 禁用多重采样抗锯齿 (MSAA)。 强烈建议你在开启SSAO时禁用MSAA，这样可以大幅度提高帧数。

-nohdr : 禁用HDR（不推荐）。

-directblit : force to blit the FrameBufferObject to screen.

-nodirectblit : force to render backbuffer as a textured quad to screen.

-hdrcolor 8/16/32 : set the HDR internal framebufferobject/texture color.

-msaa 4/8/16 : 多重采样抗锯齿的采样倍数.

#### Console Vars

r_hdr 1 / 0 : 开启/关闭 HDR后处理. 推荐值 : 1

r_hdr_blurwidth :设置HDR的模糊强度. 推荐值 : 0.1

r_hdr_exposure : 控制HDR的曝光程度. 推荐值 : 5

r_hdr_darkness : 控制HDR的明暗程度. 推荐值 : 4

r_hdr_adaptation : 控制HDR的明暗适应速度. 推荐值 : 50

r_water 2 / 1 / 0 : 控制是否开启水面反射/折射。 2 = 反射所有物体, 1 = 仅反射地形. 推荐值 : 1

r_water_fresnel (0.0 ~ 2.0) : 控制菲涅尔效应强度，如何混合折射和反射画面. 推荐值 : 1.5

r_water_depthfactor (0.0 ~ 1000.0) : 控制距离水底的深度对水体透明度的影响，原本完全不透明的水体不受此参数影响永远保持完全不透明. 推荐值 : 50

r_water_normfactor (0.0 ~ 1000.0) : 控制水面波纹大小. 推荐值 : 1.5

r_water_novis 1 / 0 : 强制让绘制折射反射贴图时关闭VIS可视区域裁剪，可能会导致性能严重下降。 推荐值 : 0

r_water_texscale (0.1 ~ 1.0) : 控制反射和折射贴图相对于原始游戏分辨率的缩放大小。推荐值 : 0.5

r_water_minheight : 小于这个高度的水体不会被应用折射和反射。 推荐值 : 7.5

r_shadow 1 / 0 : 开启/关闭 逐对象阴影. 推荐值 : 1

r_shadow_angle_pitch (0.0 ~ 360.0) : 控制阴影投射源的倾角（或叫攻角）(pitch).

r_shadow_angle_yaw (0.0 ~ 360.0) : 控制阴影投射源的左右偏转角(yaw).

r_shadow_angle_roll (0.0 ~ 360.0) : 控制阴影投射源的滚动旋转角（roll）.

r_shadow_high_texsize (必须是4的平方) : 高质量阴影贴图的贴图大小. 推荐值 : 2048

r_shadow_high_distance : 这个距离内的实体使用高质量阴影贴图. 推荐值 : 400

r_shadow_high_scale : 使用高质量阴影贴图的实体的缩放大小，缩放过大可能导致一些体型较大的实体阴影出现渲染错误，缩放过小会严重影响阴影质量。 推荐值 : 4.0

r_shadow_medium_texsize : 中等质量阴影贴图的贴图大小. 推荐值 : 2048

r_shadow_medium_distance : 这个距离内的实体使用中等质量阴影贴图. 推荐值 : 1024

r_shadow_medium_scale : 使用中等质量阴影贴图的实体的缩放大小 推荐值 : 2.0

r_shadow_low_texsize : 低等质量阴影贴图的贴图大小. 推荐值 : 2048

r_shadow_low_distance : 这个距离内的实体使用中等质量阴影贴图. 推荐值 : 4096

r_shadow_low_scale : 使用低质量阴影贴图的实体的缩放大小 推荐值 : 0.5

r_ssao 1 / 0 : 开启关闭屏幕空间遮蔽（SSAO）. 推荐值 : 1

r_ssao_intensity : SSAO阴影的强度. recommended value : 0.6

r_ssao_radius : SSAO阴影的采样半径. recommended value : 30.0

r_ssao_blur_sharpness : SSAO阴影的锐化程度， 推荐值 : 1.0

r_ssao_bias : 功能未知. 推荐值 : 0.2

r_ssao_studio_model : 0 / 1 SSAO是否对mdl模型本身生效. 推荐值 : 0

r_light_dynamic 1 / 0 : 开启/关闭延迟光照和动态光源. 推荐值 : 1

r_flashlight_cone : 手电筒光锥的锥体夹角cosine值. 推荐值 : 0.9

r_flashlight_distance : 手电筒照明距离. 推荐值 : 2000.0

r_light_ambient : 动态光照的环境光强度. recommended value : 0

r_light_diffuse : 动态光照的漫反射光强度. recommended value : 0.5

r_light_specular : 动态光照的高光反光强度. recommended value : 0.1

r_light_specularpow : 动态光照的高光反光强度. recommended value : 10.0

r_studio_vbo 1 / 0 : 开启/关闭mdl模型的VBO优化和DrawCall合批优化. 推荐值 : 1

r_wsurf_vbo 1 / 0 : 开启/关闭bsp地形的VBO优化和DrawCall合批优化. 推荐值 : 1

r_fxaa 1 / 0  : 开启快速近似抗锯齿 (FXAA) ，仅当 MSAA 不可用时有效. 推荐值 : 1