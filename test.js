const test = require('brittle')
const { tcpCat } = require('.')

const REQUEST = 'GET / HTTP/1.1\r\nHost: cloudflare.com\r\nConnection: close\r\n\r\n'

test('throws ERROR_ARGUMENT_COUNT with no arguments', async (t) => {
  try {
    await tcpCat()
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_ARGUMENT_COUNT')
  }
})

test('throws ERROR_ARGUMENT_COUNT with one argument', async (t) => {
  try {
    await tcpCat('1.1.1.1')
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_ARGUMENT_COUNT')
  }
})

test('throws ERROR_ARGUMENT_COUNT with two arguments', async (t) => {
  try {
    await tcpCat('1.1.1.1', 80)
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_ARGUMENT_COUNT')
  }
})

test('throws ERROR_ARGUMENT_COUNT with four arguments', async (t) => {
  try {
    await tcpCat('1.1.1.1', 80, REQUEST, 'extra')
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_ARGUMENT_COUNT')
  }
})

test('throws ERROR_ARGUMENT_TYPE when first argument is not a string', async (t) => {
  try {
    await tcpCat(123, 80, REQUEST)
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_ARGUMENT_TYPE')
  }
})

test('throws ERROR_ARGUMENT_TYPE when second argument is not a number', async (t) => {
  try {
    await tcpCat('1.1.1.1', '80', REQUEST)
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_ARGUMENT_TYPE')
  }
})

test('throws ERROR_ARGUMENT_TYPE when third argument is not a string', async (t) => {
  try {
    await tcpCat('1.1.1.1', 80, 123)
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_ARGUMENT_TYPE')
  }
})

test('throws ERROR_PORT_OUT_OF_RANGE when port is 0', async (t) => {
  try {
    await tcpCat('1.1.1.1', 0, REQUEST)
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_PORT_OUT_OF_RANGE')
  }
})

test('throws ERROR_PORT_OUT_OF_RANGE when port is greater than 65535', async (t) => {
  try {
    await tcpCat('1.1.1.1', 65536, REQUEST)
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_PORT_OUT_OF_RANGE')
  }
})

test('throws ERROR_PORT_OUT_OF_RANGE when port is negative', async (t) => {
  try {
    await tcpCat('1.1.1.1', -1, REQUEST)
    t.fail('expected to throw')
  } catch (err) {
    t.is(err.code, 'ERROR_PORT_OUT_OF_RANGE')
  }
})

test('rejects on invalid IP address', async (t) => {
  try {
    await tcpCat('not.an.ip.address', 80, REQUEST)
    t.fail('expected to reject')
  } catch (err) {
    t.ok(err.code, 'has an error code')
  }
})

test('rejects when the peer refuses the connection', async (t) => {
  try {
    await tcpCat('127.0.0.1', 1, REQUEST)
    t.fail('expected to reject')
  } catch (err) {
    t.ok(err.code, 'has an error code')
  }
})

test('resolves with a non-empty ArrayBuffer for HTTP GET to 1.1.1.1:80', async (t) => {
  const response = await tcpCat('1.1.1.1', 80, REQUEST)
  t.ok(response instanceof ArrayBuffer, 'response is an ArrayBuffer')
  t.ok(response.byteLength > 0, 'response has bytes')
})

test('two concurrent calls both resolve independently', async (t) => {
  const [a, b] = await Promise.all([tcpCat('1.1.1.1', 80, REQUEST), tcpCat('1.1.1.1', 80, REQUEST)])
  t.ok(a.byteLength > 0, 'first response has bytes')
  t.ok(b.byteLength > 0, 'second response has bytes')
})
