package db

import (
	"fmt"
	"testing"
)

var address = []string{"127.0.0.1:30001"}

func TestConnection(t *testing.T) {
	NewRedisClient(address)
}

func TestGetter(t *testing.T) {
	var redisClient *RedisClient = NewRedisClient(address)
	var key = "cr1025"

	var value string
	err := redisClient.Get(key, &value)
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println("value: " + value)
}

func TestSetter(t *testing.T) {
	var redisClient *RedisClient = NewRedisClient(address)
	var key = "test-key"
	var value = "test-value"

	err := redisClient.Set(key, value, 0)
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	value = ""
	err = redisClient.Get(key, &value)
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println("value: " + value)
}
