const t = require('tap')
let runArgs
const npm = {
  commands: {
    'run-script': (args, cb) => {
      runArgs = args
      cb()
    },
  },
}
const Restart = require('../../lib/restart.js')
const restart = new Restart(npm)
t.equal(restart.usage, 'npm restart [-- <args>]')
restart.exec(['foo'], () => {
  t.match(runArgs, ['restart', 'foo'])
  t.end()
})
