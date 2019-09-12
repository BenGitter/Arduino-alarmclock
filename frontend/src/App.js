import React, { Component } from 'react';
import {
  MuiPickersUtilsProvider,
  KeyboardTimePicker,
} from '@material-ui/pickers';
import DateFnsUtils from '@date-io/date-fns';

import Switch from '@material-ui/core/Switch';
import FormGroup from '@material-ui/core/FormGroup';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Paper from '@material-ui/core/Paper';

import ErrorOutline from '@material-ui/icons/ErrorOutline';

class App extends Component {
  state = {
    datetime: new Date(),
    loading: true,
    error: false,
    alarm: {
      active: false,
      hours: 0,
      minutes: 0,
    },
  };

  // error_outline
  render() {
    if (this.state.loading) return <div>LOADING</div>;
    if (this.state.error)
      return (
        <div>
          <ErrorOutline fontSize="large" />
          <p className="ErrorText">Server Connection Error</p>
        </div>
      );
    let hours = this.state.alarm.hours;
    let minutes = this.state.alarm.minutes;
    if (hours < 10) hours = '0' + hours;
    if (minutes < 10) minutes = '0' + minutes;

    return (
      <div className="App">
        <Paper className="Form">
          <FormGroup>
            <MuiPickersUtilsProvider utils={DateFnsUtils}>
              <KeyboardTimePicker
                margin="normal"
                ampm={false}
                id="time-picker"
                label="Alarm time"
                value={this.state.datetime}
                onChange={this.handleDateTimeChange}
                KeyboardButtonProps={{
                  'aria-label': 'change time',
                }}
              />
            </MuiPickersUtilsProvider>

            <FormControlLabel
              control={
                <Switch
                  checked={this.state.alarm.active}
                  onChange={this.handleActiveChange}
                  color="primary"
                />
              }
              label={this.state.alarm.active ? 'ON' : 'OFF'}
            />
          </FormGroup>
        </Paper>
      </div>
    );
  }

  handleDateTimeChange = datetime => {
    const hours = datetime.getHours();
    const minutes = datetime.getMinutes();
    this.setState(
      { datetime, alarm: { ...this.state.alarm, hours, minutes } },
      this.updateServerState
    );
  };

  handleActiveChange = e => {
    this.setState(
      {
        alarm: {
          ...this.state.alarm,
          active: e.target.checked,
        },
      },
      this.updateServerState
    );
  };

  componentDidMount() {
    fetch('http://localhost:3001/api/alarm')
      .then(res => res.json())
      .then(data => {
        const _date = new Date();
        _date.setHours(data.alarm.hours);
        _date.setMinutes(data.alarm.minutes);

        this.setState({ datetime: _date, alarm: data.alarm, loading: false });
      })
      .catch(err => {
        console.log(err);
        this.setState({ loading: false, error: true });
      });
  }

  updateServerState() {
    fetch('http://localhost:3001/api/alarm', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(this.state.alarm),
    })
      .then(res => res.json())
      .then(data => {
        console.log(data);
        const _date = new Date();
        _date.setHours(data.alarm.hours);
        _date.setMinutes(data.alarm.minutes);
        _date.setSeconds(0);

        this.setState({ datetime: _date, alarm: data.alarm });
      })
      .catch(err => {
        console.log(err);
        this.setState({ loading: false, error: true });
      });
  }
}

export default App;
