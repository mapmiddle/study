package main

import (
	"fmt"
	"study/seminar/db/redis"
)

func main() {
	fmt.Println("Hello World")

	var redisClient *db.RedisClient = db.NewRedisClient([]string{"127.0.0.1:30001"})
	fmt.Println(redisClient)
}
