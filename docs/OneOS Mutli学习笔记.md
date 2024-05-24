# OneOS Multi 学习笔记



OneOS Multi版本，内部代号齐云，是oneos发布的支持带MMU设备的RTOS操作系统。

OneOS Lite 和Multi（齐云）的区别

|            | oneos Lite                                            | Multi（齐云）                                                |
| ---------- | ----------------------------------------------------- | ------------------------------------------------------------ |
| 版本发布   | 用户的应用程序和os链接成一个bin                       | 用户的应用程序和os链接成一个bin                              |
| 多应用支持 | 逻辑上多应用                                          | 每个应用一个独立的bin<br />不需要关注其他应用的链接地址      |
| 全局变量   | 直接访问，无隔离                                      | 应用1不能访问应用2的全局变量；<br />应用不能访问内核的全局变量<br />内核不能访问应用的全局变量 |
| 函数       | 直接访问，无隔离                                      | 应用1不能直接调用应用2的函数<br />应用不能直接调用内核的函数，只能通过特定汇编指令和寄存器传参到内核<br />内核不能直接调用应用的函数 |
| 驱动       | 直接访问物理地址                                      | 通过mmap映射                                                 |
| 故障隔离   | 应用bug导致整系统崩溃                                 | 应用bug只影响本应用                                          |
| 中断       | 直接使用中断isr                                       | 内核态ko或者uio                                              |
| 进程/线程  | 只有内核态线程                                        | 内核态线程/用户态线程/进程                                   |
| 应用场景   | 简单架构cortex-m<br />无mmu或固定平铺mmu<br />8k-128M | 复杂架构cortex-a<br />有mmu<br />32M-4G                      |



## 1. 构建



工具链要用专用的（OneOSCube_3.0.1_compitalble_setup-x86_64.exe），目前还没发布。

构建指令：

```
> oos help                                                             
usage: oos COMMAND [ARGS]                                              
The complete list of recognized oos commands are:                      
  project  Initialize a project                                        
  build    Compile a project                                           
  config   Do menuconfig                                               
  init     Adapt the IDE and update the compiled file                  
  clean    Remove specified targets and dependencies                   
  pack     Component Management Methods                                
  help     Display detailed help on a command                          
                                                                       
See 'oos help <command>' for more information on a specific command    
                                                                       
General Options:                                                       
  -V, --version Show version and exit.                                 
```



### 1.1 先总结

OneOS Multi 采用了yaml格式的文本来定制客户的编译特性。**YAML** 是一种较为人性化的**数据序列化语言**，写作和读写起来非常直观。

之所以能够用这种编译系统，是oneos-cube中开发了一系列解析这种格式文档并启动scons编译的脚本，这些脚本使用了python的很多特性，学习起来蛮有意思的。

简单来说，主要是工程目录下的如下几个文件（注意，不要修改文件名）

settings-gcc.yaml  ---  这个是工程编译的配置文件，相当于oneos2.x种的 osconfig.py  文件

weave.yaml           ---  代码文件夹下



支持解析的各种键值（core\scripts\buildhelper\oneoslib\weave.py）的weave.yaml (scripts\template\sample-weave.yaml)参考：

```
## weave.yaml
group_name: app

depend_macro:
  - MAC1
  - MAC2

# Conditional expressions are supported
build_option:
  cpppath:  # The list of directories that the C preprocessor will search for include directories。
    - 'abc'
  local_cpppath:  # The list of directories that the C preprocessor will search for include directories of this group。
    - 'abc'
  libs: # A list of one or more libraries that will be linked with any executable programs。
    - 'abc'
    - 'abc'
  libpath:  # The list of directories that will be searched for libraries.
    - 'abc'
    - 'abc'
  cppdefines: # Preprocessor definitions for the different variants.
    - 'abc'
    - 'bcd'
  local_cppdefines: # Preprocessor definitions of this group for the different variants.
    - 'abc'
  asdefines:  # Conditional assembly control symbols.
    - 'abc'
  ccflags:  # General options that are passed to the C and C++ compilers.
    - 'abc'
  local_ccflags:  # General options of this group that are passed to the C and C++ compilers.
    - 'abc'
  asflags:  # General options passed to the assembler.
    - 'abc'
  local_asflags:  # General options of this group passed to the assembler.
    - 'abc'
  linkflags:  # General user options passed to the linker.
    - 'abc'

# Conditional expressions are supported
source_file:
  - goodbye.c
  - good.c?{is_compiler('gcc')}
  - bye.c?{is_define(['OS_USING_SYS_HEAP', 'OS_USING_MEM_HEAP'])}

# Conditional expressions are supported
add_subdirectory:
  - hello
  - world

```



### 1.2 代码下根目录文件的作用

.clang-format 用于格式化代码，使用 clang-format 这个工具，vscode IDE安装 clang-format 这个插件后设置

[![clang-format.png](https://z4a.net/images/2024/01/03/clang-format.png)](https://z4a.net/image/jAQYkC)

简单看一下我司这个.clang-format文件（yaml文档）格式化的要求：

```
---
Language:        Cpp
# BasedOnStyle:  LLVM          # 基于 LLVM 格式化规范

# 访问说明符(public、private等)的偏移
AccessModifierOffset: -2       

#开括号(开圆括号、开尖括号、开方括号)后的对齐: Align, DontAlign, AlwaysBreak(总是在开括号后换行)
AlignAfterOpenBracket: Align


AlignConsecutiveMacros: true

# 连续赋值时，对齐所有等号
AlignConsecutiveAssignments: true

# 连续声明时，对齐所有声明的变量名
AlignConsecutiveDeclarations: true

# 左对齐逃脱换行(使用反斜杠换行)的反斜杠
AlignEscapedNewlines: Right

# 水平对齐二元和三元表达式的操作数
AlignOperands: true

# 对齐连续的尾随的注释
AlignTrailingComments: true


AllowAllArgumentsOnNextLine: false


AllowAllConstructorInitializersOnNextLine: false

# 不允许函数声明的所有参数在放在下一行
AllowAllParametersOfDeclarationOnNextLine: false

# 不允许短的块放在同一行
AllowShortBlocksOnASingleLine: false

# 不允许短的case标签放在同一行
AllowShortCaseLabelsOnASingleLine: false

# 允许短的函数放在同一行: None, InlineOnly(定义在类中), Empty(空函数), Inline(定义在类中，空函数), All
AllowShortFunctionsOnASingleLine: None


AllowShortLambdasOnASingleLine: All

# 不允许短的if语句保持在同一行
AllowShortIfStatementsOnASingleLine: Never

# 不允许短的循环保持在同一行
AllowShortLoopsOnASingleLine: false

# 总是在定义返回类型后换行(deprecated)
AlwaysBreakAfterDefinitionReturnType: None

# 总是在返回类型后换行: None, All, TopLevel(顶级函数，不包括在类中的函数), 
#   AllDefinitions(所有的定义，不包括声明), TopLevelDefinitions(所有的顶级函数的定义)
AlwaysBreakAfterReturnType: None

# 不在多行string字面量前换行
AlwaysBreakBeforeMultilineStrings: false

# 总是在template声明后换行
AlwaysBreakTemplateDeclarations: MultiLine

# false表示函数实参要么都在同一行，要么都各自一行
BinPackArguments: false

# false表示所有形参要么都在同一行，要么都各自一行
BinPackParameters: false

# 大括号换行，只有当BreakBeforeBraces设置为Custom时才有效
BraceWrapping:

  # class定义后面
  AfterCaseLabel:  true
  
  # 控制语句后面
  AfterClass:      true
  
  # 控制语句后面
  AfterControlStatement: true
  
  # enum定义后面
  AfterEnum:       true
  
  # 函数定义后面
  AfterFunction:   true
  
  # 命名空间定义后面
  AfterNamespace:  true
  
  # ObjC定义后面不换行
  AfterObjCDeclaration: false
  
  # struct定义后面
  AfterStruct:     true
  
  # union定义后面
  AfterUnion:      true
  
  # extern块后不换行
  AfterExternBlock: false
  
  # catch之前换行
  BeforeCatch:     true
  
  # else之前换行
  BeforeElse:      true

  # 不缩进大括号
  IndentBraces:    false
  
  #
  SplitEmptyFunction: true
  
  #
  SplitEmptyRecord: true
  
  #
  SplitEmptyNamespace: false
  
# 在二元运算符前换行: None(在操作符后换行), NonAssignment(在非赋值的操作符前换行), All(在操作符前换行)
BreakBeforeBinaryOperators: None

# 在大括号前换行: Attach(始终将大括号附加到周围的上下文), Linux(除函数、命名空间和类定义，与Attach类似), 
#   Mozilla(除枚举、函数、记录定义，与Attach类似), Stroustrup(除函数定义、catch、else，与Attach类似), 
#   Allman(总是在大括号前换行), GNU(总是在大括号前换行，并对于控制语句的大括号增加额外的缩进), WebKit(在函数前换行), Custom
#   注：这里认为语句块也属于函数
BreakBeforeBraces: Custom
...
```



.clang-format-ignore 顾名思义，这个文件中标识的文件不进行格式化。这个文件是用于提交文件到gitee上做CI格式化排除用的，VS Code文件保存自动格式化无效

weave.yaml 根目录编译构建文件

```
# 工具链配置
tool:
  prefix: 'aarch64-none-elf-'  # 工具前缀

# 编译选项（支持条件表达式）
option:
  cflags: # General options that are passed to the C compiler (C only; not C++).
    - '-mcpu=cortex-a35'
    - '-Wno-multichar -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wno-nonnull-compare -fno-common -fno-stack-protector -fno-omit-frame-pointer -Wno-sign-compare'
    - '-Dgcc --std=gnu11 -Werror-implicit-function-declaration -Wwrite-strings -Wno-implicit-fallthrough -Wno-old-style-declaration -Wno-unused-variable -Wno-unused-but-set-variable -Wno-type-limits -Wno-cast-function-type -Wno-missing-braces -Wno-discarded-qualifiers -Wno-comment'
    - '-O0 -gdwarf-2 -g ? {is_mode("O0")}'
    - '-O2 ? {is_mode(''O2'')}'
    - '-Os ? {is_mode(''Os'')}'
  asflags:  # General options passed to the assembler.
    - '-c'
    - '-mcpu=cortex-a35'
    - '-Wextra -Wall -Werror=return-type -Wshadow -Wdouble-promotion -Wno-multichar -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wno-nonnull-compare -fno-common -fno-stack-protector -fno-omit-frame-pointer'
    - '-DASSEMBLY'
    - '-gdwarf-2 ? {is_mode(''O0'')}'
  linkflags:  # General user options passed to the linker.
    - '-mcpu=cortex-a35'
    - '-nostdlib -Lexternal -nostartfiles -Wl,--gc-sections,-Map=oneos.map,-cref,-u,_start -z max-page-size=4096 -T ./linker.lds'
  libpath:
    - '$OS_ROOT/drivers/link/'
action:
  prebuild:
    - 'script: from oneoslib.build_tools import *;SConscript("$OS_ROOT/kernel/include/dummy.py")'
    - 'script: from oneoslib.build_tools import *; SConscript("$OS_ROOT/kernel/source/load/load.py")'
    - '$CC -E -x c -I. board/linker_scripts/link.lds | grep -v "^\#" > linker.lds'  
  linkpreaction:
    - 'import sys;sys.path.append("$OS_ROOT" + "/drivers/hal/rk/sdk/rk3308_hal/scripts/");import redefine;redefine.run()'

```



OneOS Multi 使用的 oos 工具编译，是实现了以oos启动命令的一些脚本，最终是用scons进行构建。之前lite版本(oneos2.x版本)有一些构建脚本在代码根目录的scripts下，齐云都放到构建工具链里（oneos-cube工具的安装目录）了，编译过程中会由oos 脚本（oos.py）生成项目目录下的SConstruct 文件。

SConstruct 文件内容

```
from oneoslib import builder

builder.start()
```

builder.py 的start函数根据settings-gcc.yaml生成了类似与Lite版本SConstruct 的内容

```

def start(env=None):
    try:
        with open('.sconsign.dblite', "rb") as f:
            pickle.load(f)
    except ValueError:
        tools.print_debug('deleting sconsign.dblite')
        os.remove('.sconsign.dblite')
    except:
        pass

    # init
    settings_suffix = ARGUMENTS.get('file', 'gcc')
    config.osconfig = SettingsWrapper(Dir('#').abspath, settings_suffix) #从settings-gcc.yaml读取配置
    optimize_level = ARGUMENTS.get('mode')
    if optimize_level is not None:
        config.osconfig.MODE = optimize_level

    osconfig = config.osconfig
    Export('osconfig')

    if env is None:
        env = Environment(tools=['mingw'],
                          AS=osconfig.AS, ASFLAGS=osconfig.ASFLAGS,
                          CC=osconfig.CC, CCFLAGS=osconfig.CFLAGS,
                          AR=osconfig.AR, ARFLAGS=osconfig.ARFLAGS,
                          CXX=osconfig.CXX, CXXFLAGS=osconfig.CXXFLAGS,
                          LINK=osconfig.LINK, LINKFLAGS=osconfig.LINKFLAGS,
                          RANLIB=osconfig.RANLIB, LIBS=osconfig.LIBS, LIBPATH=osconfig.LIBPATH)

        if osconfig.COMPILER == 'iar':
            env.Replace(CCCOM=['$CC $CCFLAGS $CPPFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS -o $TARGET $SOURCES'])
            env.Replace(ARFLAGS=[''])
            env.Replace(LINKCOM=env["LINKCOM"] + ' --map ' + osconfig.RESULT_NAME + '.map')
        elif osconfig.COMPILER == 'gcc':
            env.Replace(ASCOM=['$AS $ASFLAGS $_CPPINCFLAGS -o $TARGET $SOURCES'])
     
    env.PrependENVPath('PATH', osconfig.COMPILER_PATH)

    env['OBJCOPY'] = osconfig.OBJCOPY
    env['SIZE'] = osconfig.SIZE
    env['PRO_ROOT'] = osconfig.PRO_ROOT
    env['OS_ROOT'] = osconfig.OS_ROOT

    # prepare building environment
    objs = SetupCompile(env, config.osconfig, has_libcpu=False) #遍历读取其他目录的yaml文件

    # make a building
    StartCompile(config.osconfig.RESULT_NAME + '.' + config.osconfig.RESULT_EXTENSION, objs)

```



小问题，我们并没有看到 settings-gcc.yaml 中有 postbuild （objcopy）的字段，但实际编译最后会有objcopy和size的动作，这是什么原因？



答案是 位于 scripts\template 下的 base-settings-*.yaml 文件实现了这个 postbuild 字段，在 settings.py 中的 SettingsWrapper 类中，最先依据编译器类型加载了这个文件。然后才是根据工程目录下的settings-gcc.yaml来update 相应的字段，这里需要注意的是，update的时候，会覆盖掉base-settings-\*.yaml文件中的相同键值的内容。因此，如果你想在settings-gcc.yaml 加一个 postbuild 字段，需要把base-settings-\*.yaml中postbuild 的字段也复制过去。



对于yaml脚本中是否定义某个宏的写法的实现，例如settings-gcc.yaml 

```
option:
  cflags: # General options that are passed to the C compiler (C only; not C++).
    - '-O2 ? {is_mode("O2")}'
```

通过tools.py的 get_expression_from 来解析 `'-O2 ? {is_mode("O2")}'` 

```
def get_expression_from(val):
    if val is None:
        return None
    ret = re.match(r'(.+)\?\s*{(.+)}', val)
    if not ret:
        return val

    print("ret: {%s}, eval: {%s}" % (ret, eval(ret.group(2).strip())))
    if ret and ret.group(2) and eval(ret.group(2).strip()):
        return ret[1].strip()
    else:
        return None
```

同理还有例如 ` - syscall ? {is_define(['OS_USING_USER_MODE'])}` 的写法。

我们通过观察这个正则表达式来理解这个语句的写法：

```
'(.+)\?\s*{(.+)}'  #() 代表正则表达式分组， .+ 1个或多个任意字符， \?不转义后面？，\s* 任意长度的空格, {表达式要在大括号内}
```

通过分析is_define函数，is_define的参数里的内容可以是一个字符串，也可以是一个字符串列表



对于编译脚本的撰写，了解了以上内容，根据模板写就好了。



## 2. oneos Multi系统启动分析

该版本操作系统的多数内核机制源于 little kernel（下文简称lk）.

下面的内容基于rk3308，记录下学习过程中遇到的问题和难点。



启动



start.S("arch\arm\arm64\source\start.S") ，对比lk，加入了一些oneos的改动。经过一系列的初始化通过entry进入C语言部分.



有些内容现在不大明白，先把明白的写到这里。

os_startup.c 中 os_kernel_init 分析

1. _k_run_init_call(OS_INIT_LEVEL_PRE_KERNEL_1)

   OS_INIT_LEVEL_PRE_KERNEL_1 级别的 init call 只有一个 。定义在 platform.c 中

   OS_INIT_CALL(low_level_init, OS_INIT_LEVEL_PRE_KERNEL_1, OS_INIT_SUBLEVEL_LOW);

```c
static os_err_t low_level_init(void)
{
    arch_early_init();

    platform_early_init();

#ifdef OS_USING_SMP
    os_kprintf("welcome to QiYun/MP\n\r\n");
#else
    os_kprintf("welcome to QiYun\r\n");
#endif

    void vm_init_preheap(uint level);
    vm_init_preheap(0);

    os_kprintf("initializing heap\r\n");
    void heap_init(void);
    heap_init();
    
    void vm_init_postheap(uint level);
    vm_init_postheap(0);

    os_arch_timer_init(30);
    
    return 0;
}
```



```c
static void arm64_cpu_early_init(void)
{
    /* set the vector base */
    ARM64_WRITE_SYSREG(VBAR_EL1, (uint64_t)&arm64_exception_base);

    /* switch to EL1 */
    unsigned int current_el = ARM64_READ_SYSREG(CURRENTEL) >> 2;
    if (current_el > 1)
    {
        arm64_el3_to_el1();
    }

    os_fiq_enable();
}

void arch_early_init(void)
{
    arm64_cpu_early_init();
}
```



armv8中除EL0以外，不同异常等拥有独立的异常向量表，它们的基地址被分别保存在VBAR_EL1、VBAR_EL2和VBAR_EL3寄存器中。

arm64_cpu_early_init 设置EL1级别的异常向量表，并切换异常级别到 EL1，打开中断

在 AArch64 中，异常级别决定特权级别.EL1操作系统内核通常被描述为*特权操作系统*。通常，一个软件（例如应用程序、操作系统内核或虚拟机管理程序）占据单个异常级别。



platform_early_init

```c
void platform_early_init(void) {
    /* initialize the interrupt controller */
    arm_gic_init();

    int cpu_count = 0;

    /* add the main memory arena */
    pmm_add_arena(&arena);

    /* reserve the first 64k of ram, which should be holding the fdt */
    os_list_node_t list = OS_LIST_INIT(list);
    pmm_alloc_range(MEMBASE, 0x10000 / PAGE_SIZE, &list);

#ifdef OS_USING_SMP
    cpu_count = OS_SMP_MAX_CPUS;

    os_kprintf("booting %d cpus\n\r", cpu_count - OS_START_CPU_ID);

    /* boot the secondary cpus using the Power State Coordintion Interface */
    unsigned long psci_call_num = 0x84000000 + 3; /* SMC32 CPU_ON */
#ifdef ARCH_ARM64
    psci_call_num += 0x40000000; /* SMC64 */
#endif

    for (int cpuid = OS_START_CPU_ID + 1; cpuid < cpu_count; cpuid++)
    {
        int val = 100000;
        /* note: assumes cpuids are numbered like MPIDR 0:0:0:N */
        int ret = psci_call(psci_call_num, cpuid, MEMORY_BASE_PHYS + KERNEL_LOAD_OFFSET, 0);
        if (ret != 0) 
        {
            os_kprintf("%s,%d\n\r", __FUNCTION__, __LINE__);
            os_kprintf("ERROR: psci CPU_ON returns %d\n", ret);
        }
        while(val--) 
        {
            //os_task_msleep(400);
        }
    }
#endif
}
```

arm_gic_init 中断控制器初始化

pmm_add_arena 注意这个函数的传参是

```
static pmm_arena_t arena = {
    .name = "ram",
    .base = MEMBASE,
    .size = MEMSIZE,
    .flags = PMM_ARENA_FLAG_KMAP,
};
```

pmm（物理内存管理者）使用一个全局变量来维护内存arena对象，把这个对象加入到了arena_list这个全局链表中，申请（pmm_alloc_pages）和释放（pmm_free）内存时都会通过这个链表头找到添加的链表对象。

static os_list_node_t arena_list = OS_LIST_INIT(arena_list);

对于RK3308H，内置64M DRAM，这里配置 MEMBASE 起始位于 0x100000 （uboot跳转到kernel的位置）, MEMSIZE为 0x3F00000 (64M-0x100000).



一个可能的猜测是，最小配置成0x100000的原因是因为rk3308的preloader占据了前1M的内存地址空间，内核运行时，preloader中有代码需要同步保持。所以这一段的内存不能被覆盖。



内核线程篇

内存分配 dlmalloc

任务创建，IPC等参考3.1.0 的使用文档。可能有些小区别





驱动篇

gdbstub



组件

目前的组件都运行在内核态



用户态进程

编译参考user目录下的README.md

shell 指令 osuser start -embed 启动打包到一起的用户态程序





# 加载可执行目标文件

要运行可执行目标文件 prog，我们可以在 Linux shell 的命令行中输入它的名字：

复制

```
linux> ./prog
```

因为 prog 不是一个内置的 shell 命令，所以 shell 会认为 prog 是一个可执行目标文件，通过调用某个驻留在存储器中称为加载器（loader）的操作系统代码来运行它。任何 Linux 程序都可以通过调用 execve 函数来调用加载器，我们将在 8.4.6 节中详细描述这个函数。加载器将可执行目标文件中的代码和数据从磁盘复制到内存中，然后通过跳转到程序的第一条指令或入口点来运行该程序。这个将程序复制到内存并运行的过程叫做**加载**。

每个 Linux 程序都有一个运行时内存映像，类似于图 7-15 中所示。在 Linux 86-64 系统中，代码段总是从地址 0x400000 处开始，后面是数据段。运行时**堆**在数据段之后，通过调用 malloc 库往上增长。（我们将在 9.9 节中详细描述 mallow 和堆。）堆后面的区域是为共享模块保留的。用户栈总是从最大的合法用户地址（ 248−1248−1 ）开始，向较小内存地址增长。栈上的区域，从地址 248248 开始，是为**内核**（kernel）中的代码和数据保留的，所谓内核就是操作系统驻留在内存的部分。

为了简洁，我们把堆、数据和代码段画得彼此相邻，并且把栈顶放在了最大的合法用户地址处。实际上，由于 .data 段有对齐要求（见 7.8 节），所以代码段和数据段之间是有间隙的。同时，在分配栈、共享库和堆段运行时地址的时候，链接器还会使用地址空间布局随机化（ASLR，参见 3.10.4 节）。虽然每次程序运行时这些区域的地址都会改变，它们的相对位置是不变的。

当加载器运行时，它创建类似于图 7-15 所示的内存映像。在程序头部表的引导下，加载器将可执行文件的片（chunk）复制到代码段和数据段。接下来，加载器跳转到程序的入口点，也就是 `_start`函数的地址。这个函数是在系统目标文件 ctrl.o 中定义的，对所有的 C 程序都是一样的。`_start` 函数调用系统启动函数 __libc_start_main，该函数定义在 libc.so 中。它初始化执行环境，调用用户层的 main 函数，处理 main 函数的返回值，并且在需要的时候把控制返回给内核。

![csapp_program_vison](C:\Users\yunzhi\Desktop\齐云过程分析\pic\csapp_program_vison.png)

下面是关于加载实际是如何工作的一个概述：Linux 系统中的每个程序都运行在一个进程上下文中，有自己的虚拟地址空间。当 shell 运行一个程序时，父 shell 进程生成一个子进程，它是父进程的一个复制。子进程通过 execve 系统调用启动加载器。加载器删除子进程现有的虚拟内存段，并创建一组新的代码、数据、堆和栈段。新的栈和堆段被初始化为零。通过将虚拟地址空间中的页映射到可执行文件的页大小的片（chunk），新的代码和数据段被初始化为可执行文件的内容。最后，加载器跳转到_start 地址，它最终会调用应用程序的 main 函数。除了一些头部信息，在加载过程中没有任何从磁盘到内存的数据复制。直到 CPU 引用一个被映射的虚拟页时才会进行复制，此时，操作系统利用它的页面调度机制自动将页面从磁盘传送到内存。



位置无关代码（Position-Independent Code，PIC）



**混合关键性系统(MCS, Mixed Criticality System)**



Jailhouse 相关

一些命令：

```
ok3568 ~ # jailhouse cell list
ID      Name                    State             Assigned CPUs           Failed CPUs   
0       ok3568-openeuler        running           0-1                                     
1       ok3568-rtos             running           2-3 
```

