import pyspark
from pyspark.sql import SparkSession, functions as f
import re

spark = SparkSession.builder.appName('A3.com').getOrCreate()
sc=spark.sparkContext

#------------------------------
#Creating RDD object from text file.
#------------------------------
file_name = "odyssey.txt"
# rdd=sc.parallelize([line for line in open(file_name, 'r').readlines()])
rdd = sc.textFile(file_name).flatMap(lambda line: line.split(" "))
rddCollect = rdd.collect()

print("Number of elements in RDD object: {}".format(len(rddCollect)))

#------------------------------
#Mapping to key value pairs
#------------------------------
rdd_mapped = rdd.map(lambda x: (re.sub('[^A-Za-z]+', '', x.lower()) ,1))
rdd_mapped_collected = rdd_mapped.collect()
print("First column of mapped RDD:", rdd_mapped_collected[:10])


#------------------------------
#Reducing to word count
#------------------------------
rdd_counted = rdd_mapped.reduceByKey(lambda x, y: x+y)
rdd_counted.saveAsTextFile("3_word_count")
# print(rdd_counted.collect()[:10])

#------------------------------
#Reducing to character count
#------------------------------

rdd_character = rdd.flatMap(lambda word: [i for i in word])
rdd_character = rdd_character.map(lambda x: (x, 1))
rdd_character_r = rdd_character.reduceByKey(lambda x, y: x+ y)
rdd_character_r.saveAsTextFile("3_character_count")