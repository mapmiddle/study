# Study
Repositories for study

# redis
## Install docker/docker-compose
- https://docs.docker.com/compose/install/


## Run
```
$ cd redis
$ docker-compose up
```
- If docker-compose is not installed:
```
$ cd redis
$ docker run -p 30001-30006:30001-30006 -it -e INITIAL_PORT=30001 --init grokzen/redis-cluster:4.0.10
```

# golang

## Requirements
- go version >= 1.14
- Redis cluster must be running before doing this.

## Getting Started
```
$ cd go/project/seminar
$ go build
$ go run main.go
```

# temp
```
sudo find /usr -name c++config.h
sudo apt-get install gcc-4.8-multilib g++-4.8-multilib
```