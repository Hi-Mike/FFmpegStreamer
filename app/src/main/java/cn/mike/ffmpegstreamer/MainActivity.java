package cn.mike.ffmpegstreamer;

import android.os.AsyncTask;
import android.os.Build;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(Build.CPU_ABI);
        new MyAsyncTask().execute();
    }

    class MyAsyncTask extends AsyncTask<Void, Void, String> {

        @Override
        protected String doInBackground(Void... params) {
            File src = new File(Environment.getExternalStorageDirectory() + File.separator + "test.mp4");
            File append = new File(Environment.getExternalStorageDirectory() + File.separator + "test2.mp3");
            File out = new File(Environment.getExternalStorageDirectory() + File.separator + "out.mp3");
//            if (src.exists())
            FFmpegUtil.append(src, append, out);
            return "success";
        }

        @Override
        protected void onPostExecute(String s) {
            Log.d("FFmpeg", "onPostExecute:" + s);
        }
    }
}