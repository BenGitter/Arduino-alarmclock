const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');

const app = express();

app.use(cors());

// AlarmClock variable
const alarm = {
  active: false,
  hours: 0,
  minutes: 0,
};

app.get('/time', (req, res) => {
  const now = new Date();
  const hours = now.getHours();
  const minutes = now.getMinutes();

  res.send(`<hours>${hours}</hours><minutes>${minutes}</minutes>`);
});

app.get('/alarm', (req, res) => {
  if (alarm.active) {
    return res.send(
      `<alarm><active>${alarm.active ? 'ON' : 'OFF'}</active><hours>${
        alarm.hours
      }</hours><minutes>${alarm.minutes}</minutes></alarm>`
    );
  }
  res.send(alarm.active ? 'ON' : 'OFF');
});

app.get('/api/alarm', (req, res) => {
  res.json({ alarm });
});

app.post('/api/alarm', bodyParser.json(), (req, res) => {
  if (typeof req.body.active != 'undefined') {
    alarm.active = Boolean(req.body.active);
  }
  if (typeof req.body.hours != 'undefined') {
    if (req.body.hours >= 0 && req.body.hours <= 23) {
      alarm.hours = req.body.hours;
    } else {
      return res.status(422).json({
        error:
          'Please make sure that the hours and minutes contain valid values',
      });
    }
  }
  if (typeof req.body.minutes != 'undefined') {
    if (req.body.minutes >= 0 && req.body.minutes <= 59) {
      alarm.minutes = req.body.minutes;
    } else {
      return res.status(422).json({
        error:
          'Please make sure that the hours and minutes contain valid values',
      });
    }
  }

  res.json({ msg: 'Values set', alarm });
});

app.listen(3001, () => {
  console.log('App started on port 3001');
});
