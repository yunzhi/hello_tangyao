# 安装

1. pip install mkdocs
2. pip install markdown2
3. pip install mkdocs-material
4. pip install pymdown-extensions

# 使用

1. mkdocs new mydocs

2. cd mydocs

3. mkdocs build

4. mkdocs serve

浏览器上查看 http://127.0.0.1:8000

# 主题

mkdoc-material

通过pip进行安装

pip install mkdocs-material

查看版本

pip show mkdocs-material

修改mkdocs.yml

theme:

name: material

# 注意

* 默认情况下，必须创建一个 index.md 作为站点首页。同时也支持README.md作为首页。如果 index.md 和 README.md 同时存在，将忽略 README.md
* 扩展名为 md 的文件，会被转化为 html。非markdown文件，会被原封拷贝，不做任何修改
