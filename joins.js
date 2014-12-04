
// I'm on a train to NC and playing around with doclayer joins/buildings. I thought it would be fun to imagine that mongo doesn't exist and think about how I would want a document layer query language to look if I were able to design it myself. To add creative constraints, I came up with an artificially complex schema that purposefully involves a lot of ObjectIds in place of document values. In a way, it's much less like a document model, and much more like a relational model, which is okay in this case because I'm trying to test the boundaries of what a document query language should be able to accomplish. In the past, I've messed around with other query languages such as cypher (neo4j) and SPARQL, both of which are more graph-oriented than document oriented, but I really feel that in the future the boundaries between graph and document modeling can, should, and will evaporate. That said, I thought a forward-looking query language should be able to accommodate as much as possible from both. What I came up with is a mixture of lisp, ruby, javascript, and sparql.

// :test/tenants
[{
  name: {
    prefixes: [ObjectId], // Hon. Dr.
    suffixes: [ObjectId], // jr. esq. PMP
    name: "tom",
    surname: "brennan"
  }
}]

// :test/buildings
[{
  address: ObjectId,
  loans: [{
    bank: ObjectId,
    apr: 0.0325,
    term: 360
  }],
  leases: [{
    tenants: [ObjectId],
    escalation: {
      increase: 0.03,
      frequency: "annual",
      start: new Date("2014/11/22")
    }
    payment: 1750,
    term: 12
  }],
  marketValue: 275000,
  appreciationRate: 5%
}]

// :test/addresses
[{
  street: "800 4th St SW",
  zip: 20024
}]

// how many buildings have a market value less than 300k that have at least one lease with an escalation less than 5%?
db.buildings.find({
  marketValue: { $lt: 300000 },
  leases: {
    $any: {
      escalation.increase: { $lt: 0.05 },
    }
  }
})

// same. Except I remember the tenant's name had a suffix....
db.buildings.find({
  marketValue: { $lt: 300000 },
  leases: {
    $any: {
      escalation.increase: { $lt: 0.05 }
      tenants: {
        $any: {
          suffixes: {
            $count: {
              $gt: 0 }}}}}}})

(find :test/buildings
  (and
    (< .marketValue 300000)
    (any? .leases
      #(and
        (any? .tenants
          #(> (count .suffixes) 0)
          #(< .escalation.increase 0.05))))))

db.buildings{$b} (
  lte($b.marketValue 300000)
  $b->address{$a}
  $b.leases{$l} (
    assoc($l "group or individual?"
      if(
        gt(count($l.tenants) 1)
        "group"
        "individual"
      )
    )
    or(
      all(
        $l.tenants{$t} (
          $t.name{$n}
          gt(count($n.suffixes{$s}) 1)
        )
      )
      gt($l.escalation.increase 0.05)
    )
  )
){
  $b [* -marketValue]
  $a [*]
  $l [tenants "group or individual?"]
  $t [name suffixes]
  $n [*]
  $s [*]
}

db.buildings{$b} (
  lte($b.marketValue 300000)
  update!($b assoc($b 'expensive' false))
  rm!($b)
)

db.buildings{$b}(
  lte($b.marketValue 300000)
){$b}(
  update!($b assoc($b 'expensive' false))
){$b}(
  remove!($b)
)

// cartesian product
db.buildings{$b}(
  assoc($b "addresses_collection" db.addresses{$a})
){
  $b [*]
  $a [*]
}

// adding to a collection
insert!(db.buildings {
  x: ['y' 'z']
})

insert!(db.buildings [{...}, {...}])

// inserting from a select
db.addresses{$a}(
  insert!(db.buildings {
    address: $a
  })
)

// add a new building for any address that isn't already assigned to a building
db.addresses{$a}(
  not(
    in(
      $a,
      db.buildings{$b}(
        $b.address{$addr}
      ){$addr}
    )
  )
){$missing}(
  insert!(db.buildings {
      address: $missing
  })
)

map([$missing filter([$a db.addresses]
                not(in($a, map([$b db.buildings] $b.address))))]
  insert!(db.buildings { address: $missing })
)


filter(db.addresses $a
  not(in($a map(db.buildings $b
        $b.address
  )))
).map($missing insert!({ address: $missing }))

test.buildings.filter($b
  $b.marketValue.lte(300000)
  $b->address($a)
  $b.leases.filter($l
    or(
      $l.tenants.all($t
        $t.name($n).suffixes($s).gt(1)
      )
      $l.escalation.increase.gt(0.05)
    )
  )
).map($b
  $b.assoc("leases" $b.leases.map(($l)
    $l.assoc("group or individual?" $l.tenants.gt(1).if("group" "individual"))
  ))
).project(
  $a [foo]
)


