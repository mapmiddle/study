package db

import (
	"sync"
)

type RedisService struct {
}

var instance *RedisService
var once sync.Once

func NewRedisService() *RedisService {
	once.Do(func() {
		instance = &RedisService{}
	})

	return instance
}
