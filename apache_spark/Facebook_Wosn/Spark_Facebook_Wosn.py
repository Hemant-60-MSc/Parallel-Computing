import pyspark
from pyspark.sql import SparkSession, functions as f


#------------------------------
#Loading dataset
#------------------------------
dataset = "facebook-wosn-links/out.facebook-wosn-links"


spark = SparkSession.builder.appName('A4.com').getOrCreate()
sc=spark.sparkContext
# print(open(dataset).readlines()[:10])
rdd = sc.textFile(dataset).flatMap(lambda line: line.split("\n"))
# print(rdd.collect()[:20])


#------------------------------
# Building graph as a set for each member
#------------------------------

# graph = {i+1:set() for i in range(63731)} # a dictionary of sets to store friends
graph = {i+1:set() for i in range(63731)} # a dictionary of sets to store friends


for entity in rdd.collect()[2:]:
    x, y , _, _ = map(int, entity.split(" "))
    graph[x].add(y)
    graph[y].add(x)




#------------------------------
# Mapping (m1, [m2 .... mn]) = {m2, ..., mn} 
#------------------------------

graph_rdd = sc.parallelize(graph)
graph_rdd = graph_rdd.map(lambda x: [((min(x, i), max(x, i)),graph[x]) for i in graph[x] ])



flattened_list = [item for sublist in graph_rdd.collect() for item in sublist]

graph_rdd = sc.parallelize(flattened_list)
# print(flattened_list[6:9])
#------------------------------
# Reducing by key (m1, [m2 ... mn]) = [intersection of sets] 
#------------------------------
rdd_counted = graph_rdd.reduceByKey(lambda x, y: x.intersection(y))
print(rdd_counted.collect()[0])
rdd_counted.saveAsTextFile("4_results")

mx_len = 0
key =None

for i, j in rdd_counted.collect():
    if len(j) > mx_len:
        mx_len = len(j)
        key = i

print("Max value is {} for key {}".format(mx_len, key))

# one2onegraph = {}

# def key_value_mapping(x):
#     res =  []
#     combinations  = sorted(list(graph[x]))
#     n = len(graph[x])  
#     for i in range(n):
#         for j in range(i+1, n):
#             res.append(
#                 ((combinations[i], combinations[j]), 1)
#             )
        
#     return res

# def generate_groups(line):
#     n = len(line)
#     res = []
#     for i in range(n):
#         for j in range(i+1, n):
#             s = 
#             res.append(
#                 ((min(line[i], line[j]), max(line[i], line[j])), 1)
#                 )
#     return res

# print(generate_groups([1,2,3,4,5,6]))

# graph_rdd = sc.parallelize(graph)
# graph_rdd = graph_rdd.map(lambda x: (x, graph[x]))
# # # graph_rdd = graph_rdd.map(lambda x: key_value_mapping(x))
# # graph_rdd = graph_rdd.map(lambda x: generate_groups(x))
# print(graph_rdd.collect()[:2])

# graph_rdd_reduced = graph_rdd.reduceByKey(lambda x, y: x+y)
# print(graph_rdd_reduced.collect()[:10])



###########################################################################################################################
# graph_rdd_reduced = graph_rdd.reduceByKey(lambda x, y: get_intersection(x, y))
# print(graph_rdd_reduced.collect()[:10])

# graph_rdd = sc.parallelize(graph)
# graph_rdd = graph_rdd.map(lambda x: (x, graph[x]))
# # graph_rdd = rdd.map(lambda x: (int(x.split(" ")[0]), int(x.split(" ")[1])))
# print(graph_rdd.collect()[:10])

# def get_intersection(x, y):
#     new_set = {i for i in graph[x]}
#     return len(new_set.intersection(graph[y]))

# graph_rdd = rdd.map(lambda x: (int(x.split(" ")[0]), int(x.split(" ")[1])))
# print(graph_rdd.collect()[0])


# for entity in rdd.collect()[2:]:
#     x, y , _, _ = map(int, entity.split(" "))
#     graph[x].add(y)
#     graph[y].add(x)
# graph = [[] for i in range(63731)]