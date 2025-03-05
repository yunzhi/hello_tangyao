rust 学习笔记



2年前，短暂学习过一段时间，没有项目使用后，就基本忘完了。

根据个人的经验，频繁使用才会熟悉这门编程语言，如C语言一样，日常工作使用了这么年，才算是得心应手。

因此，我打算将对rust学习的笔记记录在这里，作为随时回来复习的一个记录。



短期来看，工作上可能用不到，就当一个兴趣吧！如果现在还不开始，就永远不会开始了。

学习计划参考 [study resource of system programming in RUST · rcore-os/rCore Wiki](https://github.com/rcore-os/rCore/wiki/study-resource-of-system-programming-in-RUST)



##### 学习方案二



1. 阅读并学习[Rust语言圣经(Rust教程 Rust Course)](https://course.rs/)（1~6天）
2. 完成配套练习[Rust语言圣经配套练习](https://practice.rs/) （1~6天）
3. 完成编程小练习[Small exercises to get you used to reading and writing Rust code!](https://github.com/rust-lang/rustlings)和中等规模练习，在实践中反复学习。（3~9天）



day1

1.  `rustup` 是 Rust 的安装程序，也是它的版本管理程序。
2. Rust 原生支持 UTF-8 编码的字符串
3. 对于 `println` 来说，我们没有使用其它语言惯用的 `%s`、`%d` 来做输出占位符，而是使用 `{}`，因为 Rust 在底层帮我们做了大量工作，会自动识别输出数据的类型，例如当前例子，会识别为 `String` 类型。
4. Rust 的集合类型不能直接进行循环，需要变成迭代器（这里是通过 `.iter()` 方法），才能用于迭代循环。
5. 变量绑定时，以下划线开始的变量没有使用时，编译可以忽略未使用告警