This is a doc website base on mkdocs.

Deply on netlify, you can visit by https://yunzhidoc.netlify.app/

Here is how to do such website:


1. mkdoc new mydocs
2. cd mydocs
3. echo "site/" > .gitignore
4. echo 3.8 > runtime.txt
5. create requirements.txt ,content:

mkdocs

mkdocs-material

pymdown-extensions

1. create netlify.toml, conten:
8. upload to you github repo
