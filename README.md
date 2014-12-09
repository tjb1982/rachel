[TOC]

# introduction

## everything is a function

Even attributes are a function of instances, so that when the source code is normalized by the compiler

```java
test.people
```

becomes 

```java
people(test)
```
and

```java
test.people(@.age.$gte(30))
```

becomes 

```java
people(test $gte(age(@) 30))
```

and either form is acceptable.

Functional closures (e.g. for use with `$filter`, `$map`, `$reduce`, and `$fun` [below]) can be expressed in three ways:

1) simply pass a function (i.e., not anonymously)
```java
coll.$filter(foo)
```
2) declare `0` or more arguments between sqare braces `[]` followed by the function body
```java
coll.$filter([item idx] idx.$mod(2).$when(item.foo))
```
3) same as 2, but use `@` argument aliases instead of specifying the arguments
```java
coll.$filter(@1.$mod(2).$when(@.foo))
```

### more examples

```java
$so-much.$fun($sum)
$so-much.$fun([arg1 arg2] $sum(arg1 arg2))
$so-much.$fun($sum(@ @1))
```


## find with $filter

Let's say you have a collection of people. Some people have children, who are other people in the same collection.

```javascript

test.people = [{
  name: "Tom",
  surname: "Brennan",
  age: 32
},{
  name: "Will",
  surname: "Wilson",
  gender: "male",
  age: 27,
  children: ["Theodora"]
}, {
  name: "Theodora"
}, {
  children: ["Theodora"],
  gender: "female"
}]
```

A simple find looks like this

```java
test.people
```

or to return only those documents that have a name

```java
test.people.$filter(name)
```

calling a collection as a function is an alias for $filter

```java
test.people(name)

test.people([person]
  person.age.gte(30)
)
```
### `@` argument alias

```java
test.people(@.age.gte(30))
```

## `$map` joins with composition

A deceptively simple case: get all the people who are known to be children. Since we have no metadata labeling people as children, we have to infer this from the parents' `children` property:

```java
test.people(children).$map(children).$cat
```

or simply

```java
test.people.$map(children).$cat
```

The `$cat` here is necessary because we don't want a list of lists, just a single list of children, so we concatenate the lists returned by each `parent.children` at the end.

However, these queries only return _references_ to the children (i.e., foreign keys), so to _dereference_ the children

```java
test.people.$map([person]
  person.children.$map([child]
    test.people([p] child.$eq(p.name)).$first
  )
).$cat

```

which can get pretty unwieldy. The solution to this problem is composition. So we define `$join` as a function (with arity overloading)

```java
$join.$fun(

  [self coll]
  self.$join(coll coll.$name)

  [self coll from]
  self.$join(coll from _id)

  [self coll from to]
  self.from.$map(
    coll(@.$eq(@@.to)).$first
  )

)
```

so that we can join two collections more easily.

```java
test.people.$map(
  @.$join(test.people children name)
).$cat
```

Compare to SQL:

```sql
SELECT * FROM test.person
WHERE person.name in (
  SELECT c.person_name FROM test.person p, test.child c
  WHERE p.name=c.person_name
)
```
Compare to Mongodb (using the nodejs native client [not ACID]):

```javascript
test.collection("people").find({
  children: { $not: { $size: 0 } }
}).toArray(function(err, docs) {
  var childList = docs.reduce(function(p1, p2) {
    return p2.children.concat(p1.children || p1);
  });
  test.collection("people").find({
    name: { $in: childList }
  }).toArray(callbackDefinedElsewhere);
});
```

Compare using new FDB DocumentLayer $deref extension:

```javascript
db.people.find({
  children: {
    $exists: true,
    $not: {$size: 0}
  }
}, {
  children: {
    $deref: {
      $coll: "test.people", // defaults to current db, collection as fieldname (e.g., "test.children")
      $from: "children",    // defaults to top parent key (e.g., "children")
      $to: "name"           // defaults to "_id"
    }
  }
})

```

## projection and aggregation

### $map with $reduce and $count

Say you have a list of buildings and you want to get the sum of their market value

```java
test.buildings.$map(marketValue).$reduce($sum)
```

Or how about the number of buildings having 5 or more tenants?

```java
test.buildings(@.tenants.$count.$gte(5)).$count
```

Now that we know the building count, get the tenants in each building

```java
test.buildings(@.tenants.$count.$gte(5)).$map(
  @.$join(test.people tenants name)
)
```

### $map with $assoc and $let

From the previous example, instead of just the tenants, get the buildings with the tenants, and an easily accessible tenant count for each building

```java
test.buildings(@.tenants.$count.$gte(5)).$map(
  @.$assoc(tenants @.$join(test.people tenants name)).$assoc(tenantCount @.tenants.$count)
)
```

One problem with this is that the tenant count is requested (and calculated?) twice for each filtered building. So use `$let` to bind the result to a symbol to be used anywhere in the body.

```java
test.buildings.$map(
  $let([c @.tenants.$count]
    c.$when(@.$assoc(tenants @.$join(test.people tenants name)).$assoc(tenantCount c))
  )
)(@)
```

## pattern matching

E.g., a simple pattern matching query using SPARQL:

```sparql
SELECT ?friend ?item WHERE {
  ?person x:knows ?friend .
  ?friend x:transaction ?transaction .
  ?transaction x:item ?item .
}
```

Get a preconstructed projection instead of a table you have to reconstruct in application code

```java
test.people.$map([person]
  person.$join(test.people knows name)(transactions).$map([friend]
    friend.$assoc(
      items
      friend.$join(test.transactions).$map([transaction]
        transaction.$join(test.catalog items)
      ).$cat
    )
  )
)
```

## indexes

create an index

```java
db.coll.$index(index-name key1 key2 key3 ... )

test.buildings.$index("buildings-by-age-and-market-value" age marketValue)
```

explicitly query with an index

```java
test.buildings.$index("buildings-by-age-and-market-value")([building] building.age.$eq(30))

test.people.$index("people-by-name")(@.name.$eq("Tom"))
```

## comments

```lisp
; this is a comment
;; so is this
```

## ontologies rather than schemata