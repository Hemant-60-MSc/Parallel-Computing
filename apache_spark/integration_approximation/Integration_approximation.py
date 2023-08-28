import pyspark
from pyspark.sql import SparkSession, functions as f
from math import pi, sin
import random

spark = SparkSession.builder.appName('A5.com').getOrCreate()
sc=spark.sparkContext

count = 1_000_000
a, b = 0, pi
rdd = sc.parallelize([random.uniform(a,b) for _ in range(count)])
rdd_mapped = rdd.map(lambda x: (1, sin(x)))
res = rdd_mapped.reduceByKey(lambda x, y: x+y)
res = res.collect()[0][1]
print("Sum of function over all values: {}".format(res))
ans = ((b-a)/float(count))*res
print("Integral approximation: {}".format(ans))
# print(res.collect())

