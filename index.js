const addon = require('bindings')('event-source-base-test')

console.log(addon.include)

const eventSource = addon.registerTestEvents(n => {
    console.log(`from callback: ${n}`)    
})

setTimeout(() => addon.unregisterTestEvents(eventSource), 20000)
console.log('Event Source')

