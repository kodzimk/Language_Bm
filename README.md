<div align="center">
<pre>
VM
---------------------------------------------------
c library to use vm as dynamic languages
</pre>
</div>

# Installation

install this repo.

```sh
git clone (link of project)
make 
```

# Usage Help

```sh
./bme -h
./basm -h
```

# Examples

```sh
%include "./examples/natives.hasm"
%bind hello "Hello, World"

push hello
push 12
plusi
push 10
write8

push hello
push 13
native write

halt
```
