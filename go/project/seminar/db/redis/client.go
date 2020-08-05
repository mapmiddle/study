package db

import (
	"encoding/json"
	"fmt"
	"time"

	"context"
	"github.com/go-redis/redis/v8"
)

var ctx = context.Background()

type RedisClient struct {
	clusterClient *redis.ClusterClient
	ctx           context.Context
}

func NewRedisClient(address []string) *RedisClient {
	clusterClient := redis.NewClusterClient(&redis.ClusterOptions{
		Addrs: address,
	})

	ctx := context.Background()
	pong, err := clusterClient.Ping(ctx).Result()
	if err != nil {
		panic(err)
	}

	fmt.Println(pong)
	// Output: PONG <nil>

	result := &RedisClient{
		clusterClient: clusterClient,
		ctx:           ctx,
	}

	return result
}

func (client *RedisClient) Get(key string, src interface{}) error {
	val, err := client.clusterClient.Get(ctx, key).Result()
	if err != nil {
		panic(err)
	}

	switch v := src.(type) {
	case *string:
		*v = string(val)

	default:
		err = json.Unmarshal([]byte(val), &src)
		if err != nil {
			panic(err)
		}
	}

	return nil
}

func (client *RedisClient) Set(key string, value interface{}, expiration time.Duration) error {
	switch v := value.(type) {
	case string:
		// do nothing

	case *string:
		value = *v

	default:
		val, err := json.Marshal(value)
		if err != nil {
			panic(err)
		}

		value = val
	}

	err := client.clusterClient.Set(ctx, key, value, expiration).Err()
	if err != nil {
		panic(err)
	}

	return nil
}
